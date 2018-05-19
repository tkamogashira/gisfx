#ifndef _LIBNKF_H_
#define _LIBNKF_H_

#ifdef __cplusplus
extern "C" {
#endif

  int nkf(const char *in, char *out, size_t len, const char *options);
  
#ifdef __cplusplus
}
#endif

#endif /* _LIBNKF_H_ */
