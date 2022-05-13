#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <threads.h>
#include "../gen/shamon.h"
#include "../gen/mmlib.h"
typedef enum __MM_STREAMCONST_ENUM { __MM_STREAMCONST_ENUM_Left, __MM_STREAMCONST_ENUM_Right } _MM_STREAMCONST_ENUM;
typedef enum __MM_EVENTCONST_ENUM { __MM_EVENTCONST_NOTHING, __MM_EVENTCONST_ENUM_hole, __MM_EVENTCONST_ENUM_Prime, __MM_EVENTCONST_ENUM_LPrime, __MM_EVENTCONST_ENUM_RPrime, __MM_EVENTCONST_ENUM_LSkip, __MM_EVENTCONST_ENUM_RSkip } _MM_EVENTCONST_ENUM;
typedef struct source_control _mm_source_control;
typedef struct __MMEV_Prime _MMEV_Prime;
typedef struct __MMEV_LPrime _MMEV_LPrime;
typedef struct __MMEV_RPrime _MMEV_RPrime;
typedef struct __MMEV_LSkip _MMEV_LSkip;
typedef struct __MMEV_RSkip _MMEV_RSkip;
size_t __mma_strm_ilen_Left = 0 ;
size_t __mma_strm_blen_Left = 0 ;
size_t __mma_strm_tlen_Left = 0 ;
size_t __mma_strm_flen_Left = 0 ;
thrd_t __mm_strm_thread_Left;
shm_arbiter_buffer * __mma_strm_buf_Left;
typedef struct __mm_strm_in_Left _mm_strm_in_Left;
typedef struct __mm_strm_out_Left _mm_strm_out_Left;
typedef struct __mm_strm_hole_Left _mm_strm_hole_Left;
_mm_strm_out_Left * __mma_strm_istrt_Left = 0 ;
_mm_strm_out_Left * __mma_strm_bstrt_Left = 0 ;
size_t __mma_strm_ilen_Right = 0 ;
size_t __mma_strm_blen_Right = 0 ;
size_t __mma_strm_tlen_Right = 0 ;
size_t __mma_strm_flen_Right = 0 ;
thrd_t __mm_strm_thread_Right;
shm_arbiter_buffer * __mma_strm_buf_Right;
typedef struct __mm_strm_in_Right _mm_strm_in_Right;
typedef struct __mm_strm_out_Right _mm_strm_out_Right;
typedef struct __mm_strm_hole_Right _mm_strm_hole_Right;
_mm_strm_out_Right * __mma_strm_istrt_Right = 0 ;
_mm_strm_out_Right * __mma_strm_bstrt_Right = 0 ;
struct __MMEV_Prime {
  int n ;
  int p ;
};
struct __MMEV_LPrime {
  int n ;
};
struct __MMEV_RPrime {
  int n ;
};
struct __MMEV_LSkip {
  int x ;
};
struct __MMEV_RSkip {
  int x ;
};
struct __mm_strm_hole_Left {
  int n ;
};
struct __mm_strm_in_Left {
  shm_event head ;
  union {
    _MMEV_Prime Prime ;
  } cases;
};
struct __mm_strm_out_Left {
  shm_event head ;
  union {
    _mm_strm_hole_Left hole ;
    _MMEV_LPrime LPrime ;
  } cases;
};
int _mm_strm_fun_Left(void * arg) {
  shm_arbiter_buffer * buffer = __mma_strm_buf_Left ;
  shm_stream * stream = shm_arbiter_buffer_stream ( buffer ) ;
  const _mm_strm_in_Left * inevent ;
  _mm_strm_out_Left * outevent ;
  while((! shm_arbiter_buffer_active ( buffer )))
  {
    sleep ( 100 ) ;
  }
  while(1)
  {
    inevent = stream_fetch ( stream,buffer ) ;
    if((inevent == 0))
    {
    }
    else
    {
    }
    
    switch (((inevent->head).kind)) {
      case __MM_EVENTCONST_ENUM_Prime:
      {
        int _mm_uv_mvar_n_0 = (((inevent->cases).Prime).n) ;
        int _mm_uv_mvar_p_1 = (((inevent->cases).Prime).p) ;
        outevent = shm_arbiter_buffer_write_ptr ( buffer ) ;
        ((outevent->head).kind) = __MM_EVENTCONST_ENUM_LPrime ;
        (((outevent->cases).LPrime).n) = _mm_uv_mvar_p_1 ;
        shm_arbiter_buffer_write_finish ( buffer ) ;
        shm_stream_consume ( stream,1 ) ;
        continue;
      }
      default:
      {
        outevent = shm_arbiter_buffer_write_ptr ( buffer ) ;
        memcpy ( outevent,inevent,sizeof ( _mm_strm_in_Left ) ) ;
        shm_arbiter_buffer_write_finish ( buffer ) ;
        shm_stream_consume ( stream,1 ) ;
      }
    }
  }
}
struct __mm_strm_hole_Right {
  int n ;
};
struct __mm_strm_in_Right {
  shm_event head ;
  union {
    _MMEV_Prime Prime ;
  } cases;
};
struct __mm_strm_out_Right {
  shm_event head ;
  union {
    _mm_strm_hole_Right hole ;
    _MMEV_RPrime RPrime ;
  } cases;
};
int _mm_strm_fun_Right(void * arg) {
  shm_arbiter_buffer * buffer = __mma_strm_buf_Right ;
  shm_stream * stream = shm_arbiter_buffer_stream ( buffer ) ;
  const _mm_strm_in_Right * inevent ;
  _mm_strm_out_Right * outevent ;
  while((! shm_arbiter_buffer_active ( buffer )))
  {
    sleep ( 100 ) ;
  }
  while(1)
  {
    inevent = stream_fetch ( stream,buffer ) ;
    if((inevent == 0))
    {
    }
    else
    {
    }
    
    switch (((inevent->head).kind)) {
      case __MM_EVENTCONST_ENUM_Prime:
      {
        int _mm_uv_mvar_n_2 = (((inevent->cases).Prime).n) ;
        int _mm_uv_mvar_p_3 = (((inevent->cases).Prime).p) ;
        outevent = shm_arbiter_buffer_write_ptr ( buffer ) ;
        ((outevent->head).kind) = __MM_EVENTCONST_ENUM_RPrime ;
        (((outevent->cases).RPrime).n) = _mm_uv_mvar_p_3 ;
        shm_arbiter_buffer_write_finish ( buffer ) ;
        shm_stream_consume ( stream,1 ) ;
        continue;
      }
      default:
      {
        outevent = shm_arbiter_buffer_write_ptr ( buffer ) ;
        memcpy ( outevent,inevent,sizeof ( _mm_strm_in_Right ) ) ;
        shm_arbiter_buffer_write_finish ( buffer ) ;
        shm_stream_consume ( stream,1 ) ;
      }
    }
  }
}
int __mm_monitor_running = 1 ;
typedef struct __MMARBTP _MMARBTP;
typedef struct __MMMONTP _MMMONTP;
struct __MMARBTP {
  int pre ;
  int seen ;
  int post ;
};
struct __MMMONTP {
  int * pbuf ;
};
int arbiterMonitor( ) {
  _MMARBTP _mm_arbiter ;
  _MMMONTP _mm_monitor ;
  (_mm_arbiter.pre) = 0 ;
  (_mm_arbiter.seen) = 0 ;
  (_mm_arbiter.post) = 0 ;
  (_mm_monitor.pbuf) = (int *) malloc(sizeof(int) * 3) ;
  while(__mm_monitor_running)
  {
    goto __mm_label_arbmon_L_ArBmOn_EQ;
    __mm_label_arbmon_L_ArBmOn_ERROR:
    printf ( "ERROR: Monitor moved to error state\n" ) ;
    exit ( 1 ) ;
    break;
    __mm_label_arbmon_L_ArBmOn_EQ:
    {
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_tlen_Left == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_EQ;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_EQ;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_4 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_4) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_EQ;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_5 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_5 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_5) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_EQ;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_6 = __mm_evfref_Right_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_EQ;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_7 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_7 ;
                {
                  int _mm_uv_mvar_n_25 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_25 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 0 ]) = _mm_uv_mvar_n_25 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_Right1;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_8 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_8 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_8 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            {
              int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
              {
              }
            }
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_9 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_9 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_9 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_9) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_10 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_10 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_10 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_10 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_10 - (_mm_arbiter.pre)) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_11 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_11 ;
                  {
                    int _mm_uv_mvar_n_24 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_24 == __mm_arbiter_yieldvar_n))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = _mm_uv_mvar_n_24 ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left1;
                    }
                    else
                    {
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_12 = __mm_evfref_Left_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_EQ;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_EQ;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_13 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_EQ;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_EQ;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state L/EQ\n" ) ;
    break;
    __mm_label_arbmon_L_ArBmOn_Left1:
    {
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_tlen_Left == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left1;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_4 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_4) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_5 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_5 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_5) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left1;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_6 = __mm_evfref_Right_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left1;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_7 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_7 ;
                {
                  int _mm_uv_mvar_n_27 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_27 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_27 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_EQ;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_28 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_28 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_28 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_L_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_28 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_28 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_28 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_L_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_8 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_8 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_8 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            {
              int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
              {
                if((__mm_arbiter_yieldvar_x == 1))
                {
                  if((! 1))
                  {
                    printf ( "Error detected. Aborting...\n" ) ;
                    exit ( 1 ) ;
                  }
                  else
                  {
                  }
                  
                  printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                  goto __mm_label_arbmon_R_ArBmOn_EQ;
                }
                else
                {
                }
                
              }
            }
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_9 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_9 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_9 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_9) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                  if((__mm_arbiter_yieldvar_x == 1))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_EQ;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_10 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_10 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_10 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_10 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_10 - (_mm_arbiter.pre)) ;
                {
                  if((__mm_arbiter_yieldvar_x == 1))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_EQ;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_11 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_11 ;
                  {
                    int _mm_uv_mvar_n_26 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_26 == __mm_arbiter_yieldvar_n))
                    {
                      ((_mm_monitor.pbuf) [ 1 ]) = _mm_uv_mvar_n_26 ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left2;
                    }
                    else
                    {
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_12 = __mm_evfref_Left_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left1;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_13 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left1;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state L/Left1\n" ) ;
    break;
    __mm_label_arbmon_L_ArBmOn_Left2:
    {
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_tlen_Left == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left2;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_4 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_4) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_5 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_5 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_5) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left2;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_6 = __mm_evfref_Right_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left2;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_7 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_7 ;
                {
                  int _mm_uv_mvar_n_30 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_30 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_30 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left1;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_31 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_31 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_31 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_L_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_31 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_31 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_31 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_L_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_8 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_8 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_8 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            {
              int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
              {
                if((__mm_arbiter_yieldvar_x == 2))
                {
                  if((! 1))
                  {
                    printf ( "Error detected. Aborting...\n" ) ;
                    exit ( 1 ) ;
                  }
                  else
                  {
                  }
                  
                  printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                  goto __mm_label_arbmon_R_ArBmOn_EQ;
                }
                else
                {
                  if((__mm_arbiter_yieldvar_x == 1))
                  {
                    ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Left1;
                  }
                  else
                  {
                  }
                  
                }
                
              }
            }
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_9 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_9 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_9 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_9) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                  if((__mm_arbiter_yieldvar_x == 2))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_EQ;
                  }
                  else
                  {
                    if((__mm_arbiter_yieldvar_x == 1))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left1;
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_10 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_10 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_10 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_10 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_10 - (_mm_arbiter.pre)) ;
                {
                  if((__mm_arbiter_yieldvar_x == 2))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_EQ;
                  }
                  else
                  {
                    if((__mm_arbiter_yieldvar_x == 1))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left1;
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_11 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_11 ;
                  {
                    int _mm_uv_mvar_n_29 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_29 == __mm_arbiter_yieldvar_n))
                    {
                      ((_mm_monitor.pbuf) [ 2 ]) = _mm_uv_mvar_n_29 ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left3;
                    }
                    else
                    {
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_12 = __mm_evfref_Left_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left2;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_13 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left2;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state L/Left2\n" ) ;
    break;
    __mm_label_arbmon_L_ArBmOn_Left3:
    {
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_tlen_Left == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left3;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_4 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_4) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_5 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_5 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_5) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left3;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_6 = __mm_evfref_Right_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left3;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_7 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_7 ;
                {
                  int _mm_uv_mvar_n_32 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_32 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_32 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left2;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_33 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_33 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_33 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_L_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_33 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_33 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_33 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_L_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_8 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_8 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_8 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            {
              int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
              {
                if((__mm_arbiter_yieldvar_x == 3))
                {
                  if((! 1))
                  {
                    printf ( "Error detected. Aborting...\n" ) ;
                    exit ( 1 ) ;
                  }
                  else
                  {
                  }
                  
                  printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                  goto __mm_label_arbmon_R_ArBmOn_EQ;
                }
                else
                {
                  if((__mm_arbiter_yieldvar_x == 2))
                  {
                    ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Left1;
                  }
                  else
                  {
                    if((__mm_arbiter_yieldvar_x == 1))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Left2;
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
                
              }
            }
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_9 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_9 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_9 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_9) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                  if((__mm_arbiter_yieldvar_x == 3))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_EQ;
                  }
                  else
                  {
                    if((__mm_arbiter_yieldvar_x == 2))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left1;
                    }
                    else
                    {
                      if((__mm_arbiter_yieldvar_x == 1))
                      {
                        ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                        ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
                        goto __mm_label_arbmon_L_ArBmOn_Left2;
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_10 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_10 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_10 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_10 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_10 - (_mm_arbiter.pre)) ;
                {
                  if((__mm_arbiter_yieldvar_x == 3))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_EQ;
                  }
                  else
                  {
                    if((__mm_arbiter_yieldvar_x == 2))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left1;
                    }
                    else
                    {
                      if((__mm_arbiter_yieldvar_x == 1))
                      {
                        ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                        ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
                        goto __mm_label_arbmon_L_ArBmOn_Left2;
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_11 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_11 ;
                  {
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_12 = __mm_evfref_Left_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left3;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_13 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left3;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state L/Left3\n" ) ;
    break;
    __mm_label_arbmon_L_ArBmOn_Right1:
    {
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_tlen_Left == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right1;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_4 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_4) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_5 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_5 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_5) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right1;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_6 = __mm_evfref_Right_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right1;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_7 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_7 ;
                {
                  int _mm_uv_mvar_n_34 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_34 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 1 ]) = _mm_uv_mvar_n_34 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_Right2;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_8 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_8 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_8 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            {
              int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
              {
              }
            }
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_9 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_9 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_9 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_9) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_10 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_10 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_10 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_10 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_10 - (_mm_arbiter.pre)) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_11 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_11 ;
                  {
                    int _mm_uv_mvar_n_35 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_35 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_35 == ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                        goto __mm_label_arbmon_L_ArBmOn_EQ;
                      }
                      else
                      {
                        int _mm_uv_mvar_n_36 = __mm_arbiter_yieldvar_n ;
                        if((_mm_uv_mvar_n_36 == __mm_arbiter_yieldvar_n))
                        {
                          if((_mm_uv_mvar_n_36 != ((_mm_monitor.pbuf) [ 0 ])))
                          {
                            if((! 0))
                            {
                              printf ( "Error detected. Aborting...\n" ) ;
                              exit ( 1 ) ;
                            }
                            else
                            {
                            }
                            
                            printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                            goto __mm_label_arbmon_L_ArBmOn_ERROR;
                          }
                          else
                          {
                          }
                          
                        }
                        else
                        {
                        }
                        
                      }
                      
                    }
                    else
                    {
                      int _mm_uv_mvar_n_36 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_36 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_36 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_L_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_12 = __mm_evfref_Left_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right1;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_13 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right1;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state L/Right1\n" ) ;
    break;
    __mm_label_arbmon_L_ArBmOn_Right2:
    {
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_tlen_Left == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right2;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_4 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_4) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_5 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_5 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_5) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right2;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_6 = __mm_evfref_Right_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right2;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_7 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_7 ;
                {
                  int _mm_uv_mvar_n_37 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_37 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 2 ]) = _mm_uv_mvar_n_37 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_Right3;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_8 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_8 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_8 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            {
              int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
              {
              }
            }
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_9 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_9 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_9 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_9) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_10 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_10 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_10 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_10 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_10 - (_mm_arbiter.pre)) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_11 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_11 ;
                  {
                    int _mm_uv_mvar_n_38 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_38 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_38 == ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
                        goto __mm_label_arbmon_L_ArBmOn_Left1;
                      }
                      else
                      {
                        int _mm_uv_mvar_n_39 = __mm_arbiter_yieldvar_n ;
                        if((_mm_uv_mvar_n_39 == __mm_arbiter_yieldvar_n))
                        {
                          if((_mm_uv_mvar_n_39 != ((_mm_monitor.pbuf) [ 0 ])))
                          {
                            if((! 0))
                            {
                              printf ( "Error detected. Aborting...\n" ) ;
                              exit ( 1 ) ;
                            }
                            else
                            {
                            }
                            
                            printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                            goto __mm_label_arbmon_L_ArBmOn_ERROR;
                          }
                          else
                          {
                          }
                          
                        }
                        else
                        {
                        }
                        
                      }
                      
                    }
                    else
                    {
                      int _mm_uv_mvar_n_39 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_39 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_39 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_L_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_12 = __mm_evfref_Left_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right2;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_13 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right2;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state L/Right2\n" ) ;
    break;
    __mm_label_arbmon_L_ArBmOn_Right3:
    {
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_tlen_Left == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right3;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_4 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_4) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_5 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_5 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_5) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right3;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_6 = __mm_evfref_Right_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right3;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_7 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_7 ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_8 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_8 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_8 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
            {
              int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
              {
              }
            }
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_9 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_9 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_9 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_9) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_10 = __mm_evfref_Right_0_n ;
          if((_mm_uv_mvar_n_10 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_10 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_10 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_10 - (_mm_arbiter.pre)) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_11 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_11 ;
                  {
                    int _mm_uv_mvar_n_40 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_40 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_40 == ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                        ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
                        goto __mm_label_arbmon_L_ArBmOn_Right2;
                      }
                      else
                      {
                        int _mm_uv_mvar_n_41 = __mm_arbiter_yieldvar_n ;
                        if((_mm_uv_mvar_n_41 == __mm_arbiter_yieldvar_n))
                        {
                          if((_mm_uv_mvar_n_41 != ((_mm_monitor.pbuf) [ 0 ])))
                          {
                            if((! 0))
                            {
                              printf ( "Error detected. Aborting...\n" ) ;
                              exit ( 1 ) ;
                            }
                            else
                            {
                            }
                            
                            printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                            goto __mm_label_arbmon_L_ArBmOn_ERROR;
                          }
                          else
                          {
                          }
                          
                        }
                        else
                        {
                        }
                        
                      }
                      
                    }
                    else
                    {
                      int _mm_uv_mvar_n_41 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_41 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_41 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_L_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_L_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_12 = __mm_evfref_Left_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right3;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_13 = __mm_evfref_Left_0_n ;
          if((__mma_strm_tlen_Right == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right3;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_13) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state L/Right3\n" ) ;
    break;
    __mm_label_arbmon_R_ArBmOn_ERROR:
    printf ( "ERROR: Monitor moved to error state\n" ) ;
    exit ( 1 ) ;
    break;
    __mm_label_arbmon_R_ArBmOn_EQ:
    {
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_tlen_Right == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_EQ;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_EQ;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_14 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_14) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_EQ;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_15 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_15 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_15) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_EQ;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_16 = __mm_evfref_Left_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_EQ;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_17 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_17 ;
                {
                  int _mm_uv_mvar_n_24 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_24 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 0 ]) = _mm_uv_mvar_n_24 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Left1;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_18 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_18 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_18 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_EQ;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_19 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_19 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_19 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_19) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_20 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_20 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_20 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_20 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_20 - (_mm_arbiter.pre)) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_21 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_21 ;
                  {
                    int _mm_uv_mvar_n_25 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_25 == __mm_arbiter_yieldvar_n))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = _mm_uv_mvar_n_25 ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Right1;
                    }
                    else
                    {
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_22 = __mm_evfref_Right_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_25 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_25 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 0 ]) = _mm_uv_mvar_n_25 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Right1;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_25 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_25 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 0 ]) = _mm_uv_mvar_n_25 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Right1;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_23 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_EQ;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_EQ;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state R/EQ\n" ) ;
    break;
    __mm_label_arbmon_R_ArBmOn_Left1:
    {
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_tlen_Right == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Left1;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_14 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_14) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_15 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_15 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_15) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Left1;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_16 = __mm_evfref_Left_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Left1;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_17 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_17 ;
                {
                  int _mm_uv_mvar_n_26 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_26 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 1 ]) = _mm_uv_mvar_n_26 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Left2;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_18 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_18 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_18 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left1;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_19 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_19 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_19 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_19) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_20 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_20 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_20 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_20 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_20 - (_mm_arbiter.pre)) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_21 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_21 ;
                  {
                    int _mm_uv_mvar_n_27 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_27 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_27 == ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_EQ;
                      }
                      else
                      {
                        int _mm_uv_mvar_n_28 = __mm_arbiter_yieldvar_n ;
                        if((_mm_uv_mvar_n_28 == __mm_arbiter_yieldvar_n))
                        {
                          if((_mm_uv_mvar_n_28 != ((_mm_monitor.pbuf) [ 0 ])))
                          {
                            if((! 0))
                            {
                              printf ( "Error detected. Aborting...\n" ) ;
                              exit ( 1 ) ;
                            }
                            else
                            {
                            }
                            
                            printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                            goto __mm_label_arbmon_R_ArBmOn_ERROR;
                          }
                          else
                          {
                          }
                          
                        }
                        else
                        {
                        }
                        
                      }
                      
                    }
                    else
                    {
                      int _mm_uv_mvar_n_28 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_28 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_28 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_22 = __mm_evfref_Right_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_27 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_27 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_27 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_EQ;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_28 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_28 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_28 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_28 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_28 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_28 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_27 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_27 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_27 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_EQ;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_28 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_28 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_28 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_28 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_28 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_28 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_23 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left1;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state R/Left1\n" ) ;
    break;
    __mm_label_arbmon_R_ArBmOn_Left2:
    {
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_tlen_Right == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Left2;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_14 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_14) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_15 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_15 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_15) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Left2;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_16 = __mm_evfref_Left_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Left2;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_17 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_17 ;
                {
                  int _mm_uv_mvar_n_29 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_29 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 2 ]) = _mm_uv_mvar_n_29 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left3\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Left3;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_18 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_18 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_18 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left2\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left2;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_19 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_19 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_19 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_19) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_20 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_20 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_20 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_20 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_20 - (_mm_arbiter.pre)) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_21 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_21 ;
                  {
                    int _mm_uv_mvar_n_30 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_30 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_30 == ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_Left1;
                      }
                      else
                      {
                        int _mm_uv_mvar_n_31 = __mm_arbiter_yieldvar_n ;
                        if((_mm_uv_mvar_n_31 == __mm_arbiter_yieldvar_n))
                        {
                          if((_mm_uv_mvar_n_31 != ((_mm_monitor.pbuf) [ 0 ])))
                          {
                            if((! 0))
                            {
                              printf ( "Error detected. Aborting...\n" ) ;
                              exit ( 1 ) ;
                            }
                            else
                            {
                            }
                            
                            printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                            goto __mm_label_arbmon_R_ArBmOn_ERROR;
                          }
                          else
                          {
                          }
                          
                        }
                        else
                        {
                        }
                        
                      }
                      
                    }
                    else
                    {
                      int _mm_uv_mvar_n_31 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_31 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_31 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_22 = __mm_evfref_Right_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_30 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_30 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_30 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Left1;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_31 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_31 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_31 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_31 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_31 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_31 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_30 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_30 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_30 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Left1;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_31 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_31 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_31 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_31 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_31 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_31 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_23 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left2;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state R/Left2\n" ) ;
    break;
    __mm_label_arbmon_R_ArBmOn_Left3:
    {
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_tlen_Right == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Left3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left3\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Left3;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_14 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_14) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_15 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_15 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_15) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left3\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Left3;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_16 = __mm_evfref_Left_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left3\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Left3;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_17 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_17 ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_18 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_18 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_18 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left3\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Left3;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_19 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_19 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_19 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_19) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_20 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_20 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_20 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_20 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_20 - (_mm_arbiter.pre)) ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_21 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_21 ;
                  {
                    int _mm_uv_mvar_n_32 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_32 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_32 == ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                        ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_Left2;
                      }
                      else
                      {
                        int _mm_uv_mvar_n_33 = __mm_arbiter_yieldvar_n ;
                        if((_mm_uv_mvar_n_33 == __mm_arbiter_yieldvar_n))
                        {
                          if((_mm_uv_mvar_n_33 != ((_mm_monitor.pbuf) [ 0 ])))
                          {
                            if((! 0))
                            {
                              printf ( "Error detected. Aborting...\n" ) ;
                              exit ( 1 ) ;
                            }
                            else
                            {
                            }
                            
                            printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                            goto __mm_label_arbmon_R_ArBmOn_ERROR;
                          }
                          else
                          {
                          }
                          
                        }
                        else
                        {
                        }
                        
                      }
                      
                    }
                    else
                    {
                      int _mm_uv_mvar_n_33 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_33 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_33 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_22 = __mm_evfref_Right_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_32 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_32 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_32 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Left2;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_33 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_33 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_33 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_33 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_33 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_33 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_32 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_32 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_32 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left2\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Left2;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_33 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_33 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_33 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_33 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_33 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_33 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_23 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left3;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left3\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Left3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state R/Left3\n" ) ;
    break;
    __mm_label_arbmon_R_ArBmOn_Right1:
    {
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_tlen_Right == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Right1;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_14 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_14) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_15 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_15 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_15) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Right1;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_16 = __mm_evfref_Left_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Right1;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_17 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_17 ;
                {
                  int _mm_uv_mvar_n_35 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_35 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_35 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_EQ;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_36 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_36 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_36 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_36 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_36 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_36 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_18 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_18 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_18 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right1;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_19 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_19 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_19 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_19) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                  if((__mm_arbiter_yieldvar_x == 1))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_EQ;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_20 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_20 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_20 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_20 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_20 - (_mm_arbiter.pre)) ;
                {
                  if((__mm_arbiter_yieldvar_x == 1))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_EQ;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_21 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_21 ;
                  {
                    int _mm_uv_mvar_n_34 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_34 == __mm_arbiter_yieldvar_n))
                    {
                      ((_mm_monitor.pbuf) [ 1 ]) = _mm_uv_mvar_n_34 ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Right2;
                    }
                    else
                    {
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_22 = __mm_evfref_Right_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_34 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_34 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 1 ]) = _mm_uv_mvar_n_34 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Right2;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_34 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_34 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 1 ]) = _mm_uv_mvar_n_34 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Right2;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_23 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right1;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right1;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state R/Right1\n" ) ;
    break;
    __mm_label_arbmon_R_ArBmOn_Right2:
    {
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_tlen_Right == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Right2;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_14 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_14) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_15 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_15 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_15) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Right2;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_16 = __mm_evfref_Left_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Right2;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_17 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_17 ;
                {
                  int _mm_uv_mvar_n_38 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_38 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_38 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Left1;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_39 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_39 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_39 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_39 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_39 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_39 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_18 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_18 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_18 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right2;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_19 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_19 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_19 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_19) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                  if((__mm_arbiter_yieldvar_x == 2))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_EQ;
                  }
                  else
                  {
                    if((__mm_arbiter_yieldvar_x == 1))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Left1;
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_20 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_20 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_20 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_20 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_20 - (_mm_arbiter.pre)) ;
                {
                  if((__mm_arbiter_yieldvar_x == 2))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_EQ;
                  }
                  else
                  {
                    if((__mm_arbiter_yieldvar_x == 1))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Left1\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Left1;
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_21 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_21 ;
                  {
                    int _mm_uv_mvar_n_37 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_37 == __mm_arbiter_yieldvar_n))
                    {
                      ((_mm_monitor.pbuf) [ 2 ]) = _mm_uv_mvar_n_37 ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Right3;
                    }
                    else
                    {
                    }
                    
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_22 = __mm_evfref_Right_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_37 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_37 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 2 ]) = _mm_uv_mvar_n_37 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Right3;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                  int _mm_uv_mvar_n_37 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_37 == __mm_arbiter_yieldvar_n))
                  {
                    ((_mm_monitor.pbuf) [ 2 ]) = _mm_uv_mvar_n_37 ;
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_Right3;
                  }
                  else
                  {
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_23 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right2;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right2;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state R/Right2\n" ) ;
    break;
    __mm_label_arbmon_R_ArBmOn_Right3:
    {
      __mma_strm_tlen_Right = shm_arbiter_buffer_peek ( __mma_strm_buf_Right,1,((void * *)(&__mma_strm_istrt_Right)),(&__mma_strm_ilen_Right),((void * *)(&__mma_strm_bstrt_Right)),(&__mma_strm_blen_Right) ) ;
      __mma_strm_flen_Right = (__mma_strm_ilen_Right + __mma_strm_blen_Right) ;
      if((__mma_strm_tlen_Right == 0))
      {
        if(((_mm_arbiter.pre) == 0))
        {
          if(((_mm_arbiter.post) == 0))
          {
            if(((_mm_arbiter.seen) == 0))
            {
              printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_L_ArBmOn_Right3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if(((_mm_arbiter.pre) == 0))
      {
        if(((_mm_arbiter.post) > 0))
        {
          if(((_mm_arbiter.seen) == 0))
          {
            (_mm_arbiter.pre) = (_mm_arbiter.post) ;
            (_mm_arbiter.post) = 0 ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Right3;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_14 = __mm_evfref_Right_0_n ;
          if(((_mm_arbiter.seen) == 0))
          {
            if(((_mm_arbiter.post) == 0))
            {
              (_mm_arbiter.pre) = ((_mm_arbiter.pre) + _mm_uv_mvar_n_14) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      __mma_strm_tlen_Left = shm_arbiter_buffer_peek ( __mma_strm_buf_Left,1,((void * *)(&__mma_strm_istrt_Left)),(&__mma_strm_ilen_Left),((void * *)(&__mma_strm_bstrt_Left)),(&__mma_strm_blen_Left) ) ;
      __mma_strm_flen_Left = (__mma_strm_ilen_Left + __mma_strm_blen_Left) ;
      if((__mma_strm_flen_Left >= 1))
      {
        _mm_strm_out_Left * __mm_evref_Left_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_bstrt_Left ) ;
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_15 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_15 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - _mm_uv_mvar_n_15) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Right3;
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_LPrime))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).LPrime).n) ;
          int _mm_uv_mvar_n_16 = __mm_evfref_Left_0_n ;
          if((1 <= (_mm_arbiter.pre)))
          {
            (_mm_arbiter.pre) = ((_mm_arbiter.pre) - 1) ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
            goto __mm_label_arbmon_R_ArBmOn_Right3;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_17 = __mm_evfref_Left_0_n ;
          if(((_mm_arbiter.pre) == 0))
          {
            if(((_mm_arbiter.seen) >= 1))
            {
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_17 ;
                {
                  int _mm_uv_mvar_n_40 = __mm_arbiter_yieldvar_n ;
                  if((_mm_uv_mvar_n_40 == __mm_arbiter_yieldvar_n))
                  {
                    if((_mm_uv_mvar_n_40 == ((_mm_monitor.pbuf) [ 0 ])))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                      ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Right2;
                    }
                    else
                    {
                      int _mm_uv_mvar_n_41 = __mm_arbiter_yieldvar_n ;
                      if((_mm_uv_mvar_n_41 == __mm_arbiter_yieldvar_n))
                      {
                        if((_mm_uv_mvar_n_41 != ((_mm_monitor.pbuf) [ 0 ])))
                        {
                          if((! 0))
                          {
                            printf ( "Error detected. Aborting...\n" ) ;
                            exit ( 1 ) ;
                          }
                          else
                          {
                          }
                          
                          printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                          goto __mm_label_arbmon_R_ArBmOn_ERROR;
                        }
                        else
                        {
                        }
                        
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  else
                  {
                    int _mm_uv_mvar_n_41 = __mm_arbiter_yieldvar_n ;
                    if((_mm_uv_mvar_n_41 == __mm_arbiter_yieldvar_n))
                    {
                      if((_mm_uv_mvar_n_41 != ((_mm_monitor.pbuf) [ 0 ])))
                      {
                        if((! 0))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_ERROR\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_ERROR;
                      }
                      else
                      {
                      }
                      
                    }
                    else
                    {
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Left_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Left_0_n = (((__mm_evref_Left_0->cases).hole).n) ;
          int _mm_uv_mvar_n_18 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_18 >= (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
          {
            (_mm_arbiter.pre) = (_mm_uv_mvar_n_18 - (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))) ;
            (_mm_arbiter.post) = 0 ;
            (_mm_arbiter.seen) = 0 ;
            __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
            printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right3\n" ) ;
            goto __mm_label_arbmon_L_ArBmOn_Right3;
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_19 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_19 >= ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
          {
            if((_mm_uv_mvar_n_19 < (((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post))))
            {
              (_mm_arbiter.pre) = ((((_mm_arbiter.pre) + (_mm_arbiter.seen)) + (_mm_arbiter.post)) - _mm_uv_mvar_n_19) ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = 0 ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_arbiter.seen) ;
                {
                  if((__mm_arbiter_yieldvar_x == 3))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_R_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_R_ArBmOn_EQ;
                  }
                  else
                  {
                    if((__mm_arbiter_yieldvar_x == 2))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right1\n" ) ;
                      goto __mm_label_arbmon_R_ArBmOn_Right1;
                    }
                    else
                    {
                      if((__mm_arbiter_yieldvar_x == 1))
                      {
                        ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                        ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right2\n" ) ;
                        goto __mm_label_arbmon_R_ArBmOn_Right2;
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
          int _mm_uv_mvar_n_20 = __mm_evfref_Left_0_n ;
          if((_mm_uv_mvar_n_20 >= (_mm_arbiter.pre)))
          {
            if((_mm_uv_mvar_n_20 < ((_mm_arbiter.pre) + (_mm_arbiter.seen))))
            {
              (_mm_arbiter.pre) = 0 ;
              (_mm_arbiter.post) = 0 ;
              (_mm_arbiter.seen) = ((_mm_arbiter.seen) - (_mm_uv_mvar_n_20 - (_mm_arbiter.pre))) ;
              __MM_BUFDROP ( __mma_strm_buf_Left,1,__mma_strm_tlen_Left,__mma_strm_flen_Left,__mma_strm_ilen_Left,__mma_strm_istrt_Left,__mma_strm_blen_Left,__mma_strm_bstrt_Left ) ;
              {
                int __mm_arbiter_yieldvar_x = (_mm_uv_mvar_n_20 - (_mm_arbiter.pre)) ;
                {
                  if((__mm_arbiter_yieldvar_x == 3))
                  {
                    if((! 1))
                    {
                      printf ( "Error detected. Aborting...\n" ) ;
                      exit ( 1 ) ;
                    }
                    else
                    {
                    }
                    
                    printf ( "Going to __mm_label_arbmon_L_ArBmOn_EQ\n" ) ;
                    goto __mm_label_arbmon_L_ArBmOn_EQ;
                  }
                  else
                  {
                    if((__mm_arbiter_yieldvar_x == 2))
                    {
                      ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                      if((! 1))
                      {
                        printf ( "Error detected. Aborting...\n" ) ;
                        exit ( 1 ) ;
                      }
                      else
                      {
                      }
                      
                      printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right1\n" ) ;
                      goto __mm_label_arbmon_L_ArBmOn_Right1;
                    }
                    else
                    {
                      if((__mm_arbiter_yieldvar_x == 1))
                      {
                        ((_mm_monitor.pbuf) [ 0 ]) = ((_mm_monitor.pbuf) [ 1 ]) ;
                        ((_mm_monitor.pbuf) [ 1 ]) = ((_mm_monitor.pbuf) [ 2 ]) ;
                        if((! 1))
                        {
                          printf ( "Error detected. Aborting...\n" ) ;
                          exit ( 1 ) ;
                        }
                        else
                        {
                        }
                        
                        printf ( "Going to __mm_label_arbmon_L_ArBmOn_Right2\n" ) ;
                        goto __mm_label_arbmon_L_ArBmOn_Right2;
                      }
                      else
                      {
                      }
                      
                    }
                    
                  }
                  
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
      if((__mma_strm_flen_Right >= 1))
      {
        _mm_strm_out_Right * __mm_evref_Right_0 = _MM_EVACCESS ( 0,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_bstrt_Right ) ;
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_RPrime))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).RPrime).n) ;
          int _mm_uv_mvar_n_21 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) < 3))
            {
              if(((_mm_arbiter.post) == 0))
              {
                (_mm_arbiter.seen) = ((_mm_arbiter.seen) + 1) ;
                __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
                {
                  int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_21 ;
                  {
                  }
                }
              }
              else
              {
              }
              
            }
            else
            {
            }
            
            int _mm_uv_mvar_n_22 = __mm_evfref_Right_0_n ;
            if(((_mm_arbiter.seen) >= 3))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                }
              }
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + 1) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              {
                int __mm_arbiter_yieldvar_n = _mm_uv_mvar_n_22 ;
                {
                }
              }
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
        if((((__mm_evref_Right_0->head).kind) == __MM_EVENTCONST_ENUM_hole))
        {
          int __mm_evfref_Right_0_n = (((__mm_evref_Right_0->cases).hole).n) ;
          int _mm_uv_mvar_n_23 = __mm_evfref_Right_0_n ;
          if((__mma_strm_tlen_Left == 0))
          {
            if(((_mm_arbiter.seen) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right3;
            }
            else
            {
            }
            
            if(((_mm_arbiter.post) > 0))
            {
              (_mm_arbiter.post) = ((_mm_arbiter.post) + _mm_uv_mvar_n_23) ;
              __MM_BUFDROP ( __mma_strm_buf_Right,1,__mma_strm_tlen_Right,__mma_strm_flen_Right,__mma_strm_ilen_Right,__mma_strm_istrt_Right,__mma_strm_blen_Right,__mma_strm_bstrt_Right ) ;
              printf ( "Going to __mm_label_arbmon_R_ArBmOn_Right3\n" ) ;
              goto __mm_label_arbmon_R_ArBmOn_Right3;
            }
            else
            {
            }
            
          }
          else
          {
          }
          
        }
        else
        {
        }
        
      }
      else
      {
      }
      
    }
    printf ( "ERROR: Monitor could not match rule in state R/Right3\n" ) ;
    break;
  }
}
int main(int argc,char * * argv) {
  initialize_events ( ) ;
  _mm_source_control * __mm_strm_sourcecontrol_Left ;
  shm_stream * __mma_strm_strm_Left = shm_stream_create ( "Left",(&__mm_strm_sourcecontrol_Left),argc,argv ) ;
  __mma_strm_buf_Left = shm_arbiter_buffer_create ( __mma_strm_strm_Left,sizeof ( _mm_strm_out_Left ),128 ) ;
  thrd_create ( (&__mm_strm_thread_Left),(&_mm_strm_fun_Left),0 ) ;
  shm_arbiter_buffer_set_active ( __mma_strm_buf_Left,1 ) ;
  _mm_source_control * __mm_strm_sourcecontrol_Right ;
  shm_stream * __mma_strm_strm_Right = shm_stream_create ( "Right",(&__mm_strm_sourcecontrol_Right),argc,argv ) ;
  __mma_strm_buf_Right = shm_arbiter_buffer_create ( __mma_strm_strm_Right,sizeof ( _mm_strm_out_Right ),128 ) ;
  thrd_create ( (&__mm_strm_thread_Right),(&_mm_strm_fun_Right),0 ) ;
  shm_arbiter_buffer_set_active ( __mma_strm_buf_Right,1 ) ;
  arbiterMonitor ( ) ;
  return 0 ;
}