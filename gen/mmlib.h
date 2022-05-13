#define _MM_EVACCESS(n,ilen,istrt,bstrt) (ilen>n?istrt+n:bstrt+(n-ilen))
#define __MM_BUFDROP(buf,n,tlen,flen,ilen,istrt,blen,bstrt) do{\
  shm_arbiter_buffer_drop(buf, n);\
  tlen -= n ;\
  flen -= n ;\
  if(ilen>=n) {\
    ilen -= n ;\
	istrt += n ;\
  } else {\
    ilen = blen - ( n - ilen );\
	istrt = bstrt + ( n - ilen );\
  }\
}while(0)
