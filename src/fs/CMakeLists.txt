set(FS_SRC
  sg.c
  xint_pretruncate.c
  )

add_library(fs OBJECT ${FS_SRC})
