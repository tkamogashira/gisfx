#ifndef _NKF_H_
#define _NKF_H_

/* UTF8 入出力 */
#define UTF8_INPUT_ENABLE
#define UTF8_OUTPUT_ENABLE

/* オプションで入力を指定した時に、文字コードを固定する */
#define INPUT_CODE_FIX

/* SunOS の cc を使うときは undef にしてください */
#define ANSI_C_PROTOTYPE

/******************************/
/* デフォルトの出力コード選択 */
/* Select DEFAULT_CODE        */
/* #define DEFAULT_CODE_JIS   */
/* #define DEFAULT_CODE_SJIS  */
/* #define DEFAULT_CODE_EUC   */
#define DEFAULT_CODE_UTF8
/******************************/

#endif /* _NKF_H_ */

