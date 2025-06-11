#include <ptext_highlighter.h>
#include <ptext_lexer.h>
#include <ptext.h>
#include <ptext_screen.h>

void highlight(char *hl, char *content, struct screenBuffer *buff, int len) {
  int i = 0;
  char color = -1;
  char past_color = -1;
  if (hl == NULL) {
    return;
  }
  while (1) {
    color = hl[i];
    if (past_color != color) {
      switch (hl[i]) {
      case TOKEN_SYMBOL:
        screenAppend(buff, SYMBOL_COLOR, SYMBOL_LEN);
        break;
      case TOKEN_KEYWORD:
        screenAppend(buff, KEYWORD_COLOR, KEYWORD_LEN);
        break;
      case TOKEN_PREPROC:
        screenAppend(buff, PREPROC_COLOR, PREPROC_LEN);
        break;
      case TOKEN_DIGIT:
        screenAppend(buff, DIGIT_COLOR, DIGIT_LEN);
        break;
      case TOKEN_COMMENT:
        screenAppend(buff, COMMENT_COLOR, COMMENT_LEN);
        break;
      case TOKEN_MATCH:
        screenAppend(buff, MATCH_COLOR, MATCH_LEN);
        break;
      case TOKEN_STRING:
        screenAppend(buff, STRING_COLOR, STRING_LEN);
        break;
      case TOKEN_END:
        return;
        break;
      default:
        screenAppend(buff, INVALID_COLOR, INVALID_LEN);
        break;
      }
      past_color = color;
    }
    if (i >= len) {
      return;
    }
    screenAppend(buff, &content[i], 1);
    i++;
  }
}

void prehighlight(char *hl, Lexer *l) {
  Token t = {0};
  size_t hllen = 0;
  int in_comment = (curbuf.rows[l->idx].in_comment);
  t.kind = TOKEN_SYMBOL;
  while (t.kind != TOKEN_END) {
    t = getNextToken(l);
    for (size_t i = 0; i < t.textlen; i++) {
      hl[i + hllen] = t.kind;
    }
    hllen += t.textlen;
  }
  int changed = (curbuf.rows[l->idx].in_comment != in_comment);
  hl[hllen] = TOKEN_END;
  if (changed && l->idx + 1 < curbuf.numrows) {
    Lexer tl = {.content = curbuf.rows[l->idx + 1].renchar,
                .cursor = 0,
                .contentlen = curbuf.rows[l->idx + 1].renlen,
                .idx = l->idx + 1};
    prehighlight(curbuf.rows[l->idx + 1].hl, &tl);
  }
}
