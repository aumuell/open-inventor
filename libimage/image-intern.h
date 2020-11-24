
#ifndef	__GL_IMAGE_INTERN_H__
#define	__GL_IMAGE_INTERN_H__
#ifdef __cplusplus
extern "C" {
#endif

int img_write(IMAGE *image, char *buffer,int count);
int img_read(IMAGE *image, char *buffer, int count);
unsigned int img_seek(IMAGE *image, unsigned int y, unsigned int z);
unsigned int img_optseek(IMAGE *image, unsigned int offset);
int img_badrow(IMAGE *image, unsigned int y, unsigned int z);
int img_getrowsize(IMAGE *image);

int img_rle_compact(unsigned short *expbuf, int ibpp,
                        unsigned short *rlebuf, int obpp, int cnt);
void img_rle_expand(unsigned short *rlebuf, int ibpp,
			unsigned short *expbuf, int obpp);
void img_setrowsize(IMAGE *image, int cnt, int y, int z);

void cvtimage(int buffer[]);
void cvtshorts(unsigned short buffer[],int n);
void cvtlongs(int buffer[],int n);

int i_errhdlr(const char *fmt, ...);

#ifdef __cplusplus
}
#endif
#endif	/* !__GL_IMAGE_H__ */
