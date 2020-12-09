add_library(image STATIC
    close.c
    filbuf.c
    flsbuf.c
    name.c
    open.c
    pix.c
    rdwr.c
    rle.c
    row.c

    image.h
    image-intern.h
)

set_property(TARGET image PROPERTY POSITION_INDEPENDENT_CODE ON)
