/*
 *
 *  Copyright (C) 2000 Silicon Graphics, Inc.  All Rights Reserved. 
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 * 
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: Silicon Graphics, Inc., 1600 Amphitheatre Pkwy,
 *  Mountain View, CA  94043, or:
 * 
 *  http://www.sgi.com 
 * 
 *  For further information regarding this notice, see: 
 * 
 *  http://oss.sgi.com/projects/GenInfo/NoticeExplan/
 *
 */

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <CC/osfcn.h>
#include <signal.h>
#include <invent.h>

#include "PlayClass.h"
//#define DEBUG

/*
 * local defines
 */
#ifndef FALSE
#define FALSE (0)
#define TRUE (!FALSE)
#endif
#ifndef ABS
#define ABS(a)		((a)>0.0?(a):-(a))
#endif
#ifndef MIN
#define	MIN(a, b)	((a)<(b)?(a):(b))
#endif
#ifndef MAX
#define	MAX(a, b)	((a)>(b)?(a):(b))
#endif
#define	RANGE(a, b1, b2)			\
						\
        ((b1)<(b2)?				\
	     ((a)<(b1)?				\
		  (b1):				\
		  ((a)>(b2)?			\
		       (b2):(a))):		\
	     ((a)<(b2)?				\
		  (b2):				\
		  ((a)>(b1)?			\
		       (b1):(a))))

#ifndef INTERP
#define INTERP(x1, x2, a) ((a)*(x2)+(1.0-(a))*(x1))
#endif


PlayClass::PlayClass(char *mname)
{
    myname = mname;
    rude    = FALSE;
    quiet  = FALSE;
    timed = FALSE;
    verbose = FALSE;
#ifdef DEBUG
    verbose = TRUE;
#endif
    caught_sigint = FALSE;
    filename = NULL;
    doneCallback = NULL;
    loop = FALSE;
    fading_in = FALSE;
    fading_out = FALSE;
    in_time = 0.0;
    out_time = 0.0;
    fade_level = 0.0;
    max_level = 1.0;
    isLive = FALSE;
    isDone = TRUE;
    sproced = FALSE;
    signal(SIGCLD,SIG_IGN); /* ensure no zombie child processes */
}


/*
 * attenuatesamps
 *
 *		tristram, 1993
 *
 * put a ramp on a buffer of samples, starting at the current fade_level
 * at a slope of 1 / fade_time.
 *
 * mode = 0 ==> fade in
 * mode = 1 ==> fade out
 * mode = 2 ==> don't adjust fade_level (just use max_level)
 *
 * this function has the side effect of setting fade_level
 *
 */

void
PlayClass::attenuatesamps( void * sampbuf, long sampsize, long nsamps,
               double secs_per_buf,
	       double max_level,
	       double * fade_level,
	       double fade_time,
	       int mode ) {

  int i;

#ifdef DEBUG
fprintf(stderr, "attenuating max_level = %f, fade_level = %f, fade_time = %f\n", max_level, *fade_level, fade_time);
#endif

  if ( verbose ) printf( "fade_level = %f\n", *fade_level );

  if (( mode != 2 ) && ( fade_time == 0.0 )) {
    if ( *fade_level == 0.0 ) *fade_level = 1.0;
    else		      *fade_level = 0.0;
    return;
  }

  for ( i = 0; i < nsamps; i++ ) {
    switch ( sampsize ) {
    case AL_SAMPLE_8:
      ((char *)sampbuf)[ i ] *= (char)(max_level * *fade_level);
      break;
    case AL_SAMPLE_16:
      ((short *)sampbuf)[ i ] *= (short)(max_level * *fade_level);
      break;
    case AL_SAMPLE_24:
      ((int32_t *)sampbuf)[ i ] *= max_level * *fade_level;
      break;
    }
    if ( mode != 2 ) {
      if ( fade_time == 0.0 ) {
	if ( mode == 0 ) { /* fading in */
	  *fade_level = 1.0;
	} else { /* fading  out */
	  *fade_level = 0.0;
	}
      } else {
	*fade_level += ( mode == 0 ? 1.0 : -1.0 )
	  * secs_per_buf / (fade_time * nsamps);
	*fade_level = RANGE( *fade_level, 0.0, 1.0 );
      }
    }
  }
}



int PlayClass::beginPlaying()
{
#ifdef DEBUG
  fprintf(stderr, "beginPlaying()\n");
#endif
    AFfilehandle audio_file;
    ALconfig audio_port_config;
    ALport audio_port;
    int errseen = 0;
    int result;

#ifdef DEBUG
  fprintf(stderr, "checkpoint 1()\n");
#endif

  if (isDone) isDone = FALSE;
  else return -1;

  isLive = TRUE;
  fading_in = FALSE;
  fading_out = FALSE;
  fade_level = 0.0;

  max_level = RANGE( max_level, 0.0, 1.0 );
  if (in_time == 0.0) fade_level = 1.0;

#ifdef DEBUG
  fprintf(stderr, "new max_level is %f\n", max_level);
#endif

    if ((fd = open(filename, O_RDONLY)) < 0)
      {
	  if (!quiet)
	    {
                fprintf(stderr, "%s: failed to open file '%s' %s\n",
                     myname, filename, strerror(errno));
            }
            errseen = 1;
      }
    /*
     * test the file descriptor to see whether we can attach an audio
     *    file handle to it
     */
    else if (AFidentifyfd(fd) < 0) 
      {
	  if (!quiet)
	    {
                fprintf(stderr,"%s: '%s' not an AIFF-C or AIFF file\n",
                     myname, filename);
            }
            errseen = 1;
      }
    /*
     * attach an audio file handle to the file descriptor
     */
    else if ((audio_file = AFopenfd(fd, "r", AF_NULL_FILESETUP)) 
	     == AF_NULL_FILEHANDLE)
      {
	  if (!quiet)
            {
                fprintf(stderr, "%s: failed to open file '%s'\n", 
			myname, filename);
            }
	  errseen = 1;
      }
    else 
      {
	  result = init_audio(audio_file, &audio_port, &audio_port_config);
	  if (result != -1)
            {
                play_audio_samps(audio_file, audio_port, audio_port_config);
            } else {
//		errseen = 1;
	    }
	  AFclosefile(audio_file);
	  ALfreeconfig(audio_port_config);

	  if (result != -1) 
            {
	        ALcloseport(audio_port);
            }
	  free(sampbuf);
      }
    if (errseen) {
#ifdef DEBUG
	  fprintf(stderr, "error! returning -1\n");
#endif
	isDone = 1;
        if (sproced) exit(0);
	return -1;
      }
    else {
#ifdef DEBUG
	fprintf(stderr, "done\n");
#endif
	if (doneCallback) {
#ifdef DEBUG
	    fprintf(stderr, "calling doneCallback\n");
#endif
	    doneCallback(userData, this);
	}
	isDone = 1;
	if (sproced) {
#ifdef DEBUG
	fprintf(stderr, "exiting zero");
#endif
	exit(0);
	}
#ifdef DEBUG
	fprintf(stderr, "after exit(0)");
#endif
	return 1;
    }
}

/*
 * initialize audio port and global state of IRIS Audio Processor
 */
int 
PlayClass::init_audio(AFfilehandle audio_file, ALport *audio_port,
		      ALconfig *ap_config)
{
#ifdef DEBUG
  fprintf(stderr, "init_audio(...)\n");
#endif
//    long pvbuf[4];
    long pvbuf[2];
    long audio_rate;
    long samp_wordsize;    
    AudioFormat_t vers;
    AudioFormat_t samp_type;

    samps_per_frame   = (int) AFgetchannels(audio_file, AF_DEFAULT_TRACK);
    file_rate         = AFgetrate(audio_file, AF_DEFAULT_TRACK);
    compression       = AFgetcompression(audio_file, AF_DEFAULT_TRACK);
    filefmt           = AFgetfilefmt(audio_file, &vers);

    AFgetsampfmt(audio_file, AF_DEFAULT_TRACK, &samp_type, &bits_per_samp);

    /*
     * need to determine whether audio is in use. if not, then we
     * can just go ahead and be "rude."
     */
    pvbuf[0] = AL_OUTPUT_COUNT;
//    pvbuf[2] = AL_MONITOR_CTL;
//    ALgetparams(AL_DEFAULT_DEVICE, pvbuf, 4);
    ALgetparams(AL_DEFAULT_DEVICE, pvbuf, 2);

//    if ((pvbuf[1] == 0) && (pvbuf[3] == AL_MONITOR_OFF)) {
    if (pvbuf[1] == 0) {
        rude = 1;
    }
    
    /* 
     * decide on output rate for the audio hardware
     */
    switch((int32_t)file_rate)
    {
        case 48000: audio_rate = AL_RATE_48000; frames_per_sec = 48000; break;
        case 44100: audio_rate = AL_RATE_44100; frames_per_sec = 44100; break;
        case 32000: audio_rate = AL_RATE_32000; frames_per_sec = 32000; break;
        case 22050: audio_rate = AL_RATE_22050; frames_per_sec = 22050; break;
        case 16000: audio_rate = AL_RATE_16000; frames_per_sec = 16000; break;
        case 11025: audio_rate = AL_RATE_11025; frames_per_sec = 11025; break;
        case 8000: audio_rate = AL_RATE_8000; frames_per_sec = 8000; break;
        default:
            if (!quiet)
            {
                fprintf(stderr, "%s: can't play data at sample rate %f\n",
                  myname, file_rate);
            }
            frames_per_sec = 44100; /* pick some arbitrary rate */
            audio_rate = AL_RATE_44100;
    }
    /*
     * determine the current output rate
     */
    pvbuf[0] = AL_OUTPUT_RATE;
    ALgetparams(AL_DEFAULT_DEVICE, pvbuf, 2);
    /*
     * if the rates are the same, all is well. if not, then we need to proceed
     * in a either a "rude" or "polite" manner.
     */
    if (pvbuf[1] != audio_rate)
    {
	if (rude)
	{
	    pvbuf[1] = audio_rate;
	    ALsetparams(AL_DEFAULT_DEVICE, pvbuf, 2);
	}
	else /*polite*/
	{
	    double tmp_rate;
	    
            if (!quiet)
            {
	        switch (pvbuf[1])
                {
                    case AL_RATE_48000: tmp_rate = 48000; break;
                    case AL_RATE_44100: tmp_rate = 44100; break;
                    case AL_RATE_32000: tmp_rate = 32000; break;
                    case AL_RATE_22050: tmp_rate = 22050; break;
                    case AL_RATE_16000: tmp_rate = 16000; break;
                    case AL_RATE_11025: tmp_rate = 11025; break;
                    case AL_RATE_8000:  tmp_rate = 8000;  break;
                 }
                 fprintf(stderr,
        "%s: '%s': adjust system output rate (currently %6.1f Hz)\n",
                    myname, filename, tmp_rate);

            }
        }
    }
    

    /*
     * decide what size blocks of samples we should read from the
     * input file and pass to ALwritesamps
     */
    if (bits_per_samp <= 8)
    {
       bytes_per_samp = 1;
       samp_wordsize  = AL_SAMPLE_8;
    }
    else if (bits_per_samp <= 16)
    {
       bytes_per_samp = 2;
       samp_wordsize  = AL_SAMPLE_16;
    }
    else if (bits_per_samp <= 24)
    {
       bytes_per_samp = 4;
       samp_wordsize  = AL_SAMPLE_24;
    }
    else
    {
        if (!quiet)
        { 
            fprintf(stderr, "%s: %s: error can't play %d bit samples\n",
                myname, filename, bits_per_samp);
        }
        return(-1);
    }

    if ((samps_per_frame != 1) && (samps_per_frame!= 2))
    {
       if (!quiet)
       {
          fprintf(stderr, "%s: %s: error can't play %d channel sample data\n",
             myname, filename, samps_per_frame);
       }
       return(-1);
    }

    /*
     * make the buffer large enough to hold 1/2 sec of audio frames
     * we add one to frames_per_sec before we divide in order to
     * correctly handle the 11025 case
     */
    secs_per_frame = 1.0 / ((double)frames_per_sec);
//    frames_per_buf = (frames_per_sec+1)/2;
  /* ok, I changed this so that now Im holding 1/4 sec of audio frames */
    frames_per_buf = (int) (frames_per_sec+15)/20;

    samps_per_buf = frames_per_buf * samps_per_frame;
    bytes_per_buf = samps_per_buf * bytes_per_samp;
    secs_per_buf  = secs_per_frame * frames_per_buf;

    sampbuf = (char *) malloc(bytes_per_buf);

    /*
     * configure and open audio port
     */
    *ap_config = ALnewconfig();
    ALsetwidth(*ap_config, samp_wordsize);
    ALsetchannels(*ap_config, samps_per_frame);

    /*
     * make the ring buffer large enough to hold 1 sec of audio samples
     */
    ALsetqueuesize(*ap_config, samps_per_buf*2);
    *audio_port = ALopenport(myname, "w", *ap_config);

    if (*audio_port != NULL) {
	return (1);
    }
    else {
	return (-1);
    }

}


/*
 * play audio sample data through the output port
 */
int
PlayClass::play_audio_samps(AFfilehandle audio_file, ALport audio_port,
			    ALconfig ap_config)
{
#ifdef DEBUG
  fprintf(stderr, "play_audio_samps(...)\n");
#endif
    int num_bufs;
    int leftover_bytes;
    int leftover_samps;
    int leftover_frames;
    int32_t samp_wordsize;
    int samp_count;
    int frame_count;
    double sec_count;
    int i;
    int samples_read;
    int frames_read;
    int done;
    int shortread;
    int total_frames;	
    int total_samps;
    int total_samp_bytes;

    char compressionname[10];
    float fileplayingtime;

    sec_count = 0.0;

    /*
     * figure out how many reads we have to do
     */
    total_frames    =  (int) AFgetframecnt(audio_file, AF_DEFAULT_TRACK);
    total_samps      =  total_frames * samps_per_frame;
    total_samp_bytes =  total_samps * bytes_per_samp;
    num_bufs         = total_samp_bytes / bytes_per_buf;
    leftover_bytes   = total_samp_bytes % bytes_per_buf;
    leftover_samps   = leftover_bytes / bytes_per_samp;
    leftover_frames  = leftover_samps / samps_per_frame;

    samp_wordsize = ALgetwidth(ap_config);

    if (!quiet)
    {
        fileplayingtime = (float)total_frames / (float)frames_per_sec;
        switch (compression)
        {
            default:
            case AF_COMPRESSION_NONE: 
                    strcpy(compressionname, "");
                    break;
            case AF_COMPRESSION_G722:
                    strcpy(compressionname, "G.722 -->");
                    break;
            case AF_COMPRESSION_G711_ALAW:
                    strcpy(compressionname, "A-law -->");
                    break;
            case AF_COMPRESSION_G711_ULAW:
                    strcpy(compressionname, "u-law -->");
                    break;
        }
    }
    if (verbose) 
    {
	printf("%s: '%s' %6.3f sec %7.1f Hz %6s %s %d-bit %s\n",
	       myname,
	       filename,
	       fileplayingtime,
	       file_rate,
	       samps_per_frame == 1 ? "mono" : "stereo",
	       compressionname,
	       bits_per_samp,
	       filefmt == AF_FILE_AIFFC ? "AIFF-C" : "AIFF"
	   );
        printf( "        total sample frames       = %d (%d bytes)\n",
                    total_frames, total_samp_bytes);
        printf( "        play data using blocksize = %d sample frames\n",
                    frames_per_buf);
        printf( "        total blocks              = %d (%d extra frames)\n",
                    num_bufs, leftover_frames); 
    }

    sec_count = 0.0;
 top:

    /*
     * move the fileptr to the beginning of the sample data
     */
    AFseekframe(audio_file, AF_DEFAULT_TRACK, 0);

    /*
     * note that there may be some pad bytes following the valid samples -
     * for example, the sample data area may be padded so that the valid 
     * samples begin on a block boundary and the sample area ends on a block
     * boundary (where blocksize is specified by the user)
     */
    done        = 0;
    shortread   = 0;
    samp_count  = 0;
    frame_count = 0; 
//    sec_count   = 0.0;

    for (i=0; i<num_bufs; i++)
    {
        samp_count  += samps_per_buf;
        frame_count += frames_per_buf;
        sec_count   += secs_per_buf;

        if (verbose) 
        {
            printf("        %d sample frames  %6.3f secs\n",
                frame_count, sec_count); 
        }
        if ((frames_read 
            = (int) AFreadframes(audio_file, AF_DEFAULT_TRACK, 
                          sampbuf, frames_per_buf)) < frames_per_buf)
        {
            if (!quiet)
            {
                fprintf(stderr, 
         "%s: warning short read for %s: expected %d frames, got %d frames\n",
                        myname, filename, frames_per_buf, frames_read);
            }
//            done++;
	    shortread++;
        }
        samples_read = frames_read * samps_per_frame;

	if ( timed && ( sec_count - in_time > play_time )) {
	  fading_in = FALSE;
	  fading_out = TRUE;
	}

	if ( !timed && !loop && ( sec_count > (double) fileplayingtime - out_time)) {
#ifdef DEBUG
  fprintf(stderr, "fading out\n");
#endif
	  fading_in = FALSE;
	  fading_out = TRUE;
	}
	   
	if (fading_in) {
	  attenuatesamps( sampbuf, samp_wordsize, samples_read, secs_per_buf,
			 max_level, &fade_level, in_time, 0 );
	  if ( fade_level >= 1.0 ) fading_in = FALSE;
	}
	if (fading_out) {
	  attenuatesamps( sampbuf, samp_wordsize, samples_read, secs_per_buf,
			 max_level, &fade_level, out_time, 1 );
	  if ( fade_level <= 0.0 ) {
#ifdef DEBUG
  fprintf(stderr, "fade level is less than zero, exiting \n");
#endif
	    fading_out = FALSE;
	    done = TRUE;
	  }
	}
	if ( ! ( fading_out || fading_in ) && ( max_level != 1.0 ))
	  attenuatesamps( sampbuf, samp_wordsize, samples_read, secs_per_buf,
			 max_level, &fade_level, out_time, 2 );


        ALwritesamps(audio_port, sampbuf, samples_read);

	if (caught_sigint) {
	    done++;
#ifdef DEBUG
	    fprintf(stderr, "caught_sigint\n");
#endif
	}

        if (shortread)
        {
          /*
           * allow the audio buffer to drain
           */
            while(ALgetfilled(audio_port) > 81920) {
               sginap(1);
            }
	    if (loop) 
	      goto top;
	    else
	      done = 1;

	}
	if (done) {
	    while(ALgetfilled(audio_port) > 0) {
		sginap(1);
	    }
	    sginap(10);
	    return(0);
	}
    }

    /*
     * play the leftovers
     */
    samp_count  += leftover_samps;
    frame_count += leftover_frames;
    sec_count   += ((double)leftover_frames) * secs_per_frame;
    if (verbose && leftover_samps>0) 
    {
        printf("        %d sample frames  %6.3f secs\n",
               frame_count, sec_count); 
    }
    if ((frames_read = 
       (int) AFreadframes(audio_file, AF_DEFAULT_TRACK, sampbuf, 
                                        leftover_frames)) < leftover_frames)
    {
        if (!quiet)
        { 
            fprintf(stderr, 
         "%s: warning short read for %s: expected %d frames, got %d frames\n",
                    myname, filename, leftover_frames, frames_read);
        }

    }
    samples_read = frames_read * samps_per_frame;


    if ( timed && ( sec_count - in_time > play_time )) {
      fading_in = FALSE;
      fading_out = TRUE;
    }

    if ( !timed && !loop && ( sec_count > (double) fileplayingtime - out_time)) {
#ifdef DEBUG
  fprintf(stderr, "fading out 2\n");
#endif
      fading_in = FALSE;
      fading_out = TRUE;
    }

    if (fading_in) {
      attenuatesamps( sampbuf, samp_wordsize, samples_read, secs_per_buf,
		     max_level, &fade_level, in_time, 0 );
      if ( fade_level >= 1.0 ) fading_in = FALSE;
    }
    if (fading_out) {
      attenuatesamps( sampbuf, samp_wordsize, samples_read, secs_per_buf,
		     max_level, &fade_level, out_time, 1 );
      if ( fade_level <= 0.0 ) {
	fading_out = FALSE;
	done = TRUE;
      }
    }
    if ( ! ( fading_out || fading_in ) && ( max_level != 1.0 ))
      attenuatesamps( sampbuf, samp_wordsize, samples_read, secs_per_buf,
		     max_level, &fade_level, out_time, 2 );

    ALwritesamps(audio_port, sampbuf, samples_read);

    /*
     * allow the audio buffer to drain
     */
    while(ALgetfilled(audio_port) > 81920) {
       sginap(1);
    }
    if (loop) goto top;

    while(ALgetfilled(audio_port) > 0) {
       sginap(1);
    }
    sginap(10);
    return(0);
}

void
PlayClass::setVerbose(int i)
{
    verbose = i;
}

void
PlayClass::setFilename(char *fname)
{
    filename = fname;
}

void
PlayClass::setQuiet(int i)
{
    quiet = i;
}

void
PlayClass::setRude(int i)
{
    rude = i;
}

void
PlayClass::setLoop(short i)
{
    loop = i;
}

void
PlayClass::setTime(double secs)
{
    play_time = secs;
    timed = TRUE;
}

void
PlayClass::setInTime(double secs)
{
    if (secs == 0.0) {
	fading_in = FALSE;
	in_time = 0.0;
    } else {
	fading_in = TRUE;
	in_time = secs;
    }
    
}

void
PlayClass::setOutTime(double secs)
{
    out_time = secs;
}


void PlayClass::setMaxOutputLevel(double lvl)
{
    max_level = lvl;
}

short PlayClass::getIsLive()
{
    return isLive;
}

short PlayClass::donep()
{
    return isDone;
}

void
PlayClass::setCallback(PlayClassCB *Callback, void *d)
{
#ifdef DEBUG
    fprintf(stderr, "setCallback\n");
#endif
    doneCallback = Callback;
    userData  = d;
}


int
PlayClass::start()
{

    short hasAudio = FALSE;
  
    if (filename == NULL) return -1;

    inventory_t* invry_p;

    while((invry_p = getinvent()) != 0) {
	if (invry_p->inv_class == INV_AUDIO) {
	  hasAudio = TRUE;
	  break;
        }
    }

    setinvent();
    endinvent();

    if (!hasAudio) return -1;
    // this is a quick check to see if there is an audio port available
    ALconfig conf = ALnewconfig();
    ALport   prt;
    if ( (prt = ALopenport( "noname", "w", conf )) != NULL ) {
	    // Close the audio port. We only opened it to check 
	    // if it was available.
	    ALcloseport(prt);
    } else {
	    return -1;
    }

	  
#ifdef DEBUG
    fprintf(stderr, "start() filename = %s\n", filename);
#endif
   /*
    *    Use the sproc(2) call to create an audio thread. The audio
    *    thread begins execution at the do_audio entry point. The 
    *    second argument to sproc, PR_SALL, allows the two processes
    *    to share all attributes. See the manual page for the sproc(2)
    *    system call for more details.
    */
    audio_pid =
      sproc((void (*)(void *))&PlayClass::go, PR_SALL, (void *) this);
   if (audio_pid < 0)
   {
       fprintf(stderr, "unable to create audio thread...aborting.\n");
       return -1;
   }
#ifdef DEBUG
     fprintf(stderr, "returning 1, audio_pid = %d\n", audio_pid);
#endif
   sproced = TRUE;
   return 1;

}

void PlayClass::go(void *obj)
{
#ifdef DEBUG
    fprintf(stderr, "go()\n");
#endif
    PlayClass *objPtr = (PlayClass *) obj;

    objPtr->beginPlaying();
#ifdef DEBUG
    fprintf(stderr, "return from beginPlaying()\n");
    fprintf(stderr, "process = %d\n", objPtr->audio_pid);
#endif
}

void
PlayClass::fadeOut()
{
    if (out_time > 0.0) {
	fading_out = TRUE;
	fading_in = FALSE;
	return;
    }
}

void
PlayClass::stop()
{

#ifdef DEBUG
      fprintf(stderr, "stop()\n");
#endif
      caught_sigint = 1;
#ifdef DEBUG
      fprintf(stderr, "waiting...");
#endif
//      pid_t pid;
//      int status;
//      pid = wait(&status);
      union wait waitStatus;
      (void) waitpid(audio_pid, (int *) (&waitStatus), 0);

#ifdef DEBUG
      fprintf(stderr, "done.\n");
#endif
    caught_sigint = 0;
    isLive = FALSE;
}

