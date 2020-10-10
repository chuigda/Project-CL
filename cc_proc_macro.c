#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define PARSE_NORMAL  0
#define PARSE_COMMENT 1

static int line_number = 0;
static char input_buffer[4096];
static char output_buffer[4096];
static char token_buffer[512];
static char rename_buffer[512];

static FILE *fp_in = NULL;
static FILE *fp_out = NULL;
static int parse_mode = PARSE_NORMAL;
static int idx = 0;
static int out_idx = 0; 
static int token_buf_idx = 0;

static int ret_value = 0;
static const char *error_reason = NULL;

/* Type renaming rule, left to user to finish. Default to strcpy. */
#define CCTY_RENAME(DST, SRC) \
    {\
        strcpy((DST), (SRC)); \
    }

/* Function renaming rule, left to user to finish. Default to strcpy. */
#define CCFN_RENAME(DST, SRC) \
    {\
        strcpy((DST), (SRC)); \
    }

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: cc_proc_macro <input> <output>\n");
        return -1;
    }
    
    fp_in = fopen(argv[1], "r");
    if (!fp_in) {
        fprintf(stderr, "cannot open input file \"%s\"\n", argv[1]);
        return -1;
    }
    
    fp_out = fopen(argv[2], "w");
    if (!fp_out) {
        fprintf(stderr, "cannot open output file \"%s\"\n", argv[2]);
        return -1;
    }

line_feed:
    if (fgets(input_buffer, 4095, fp_in) == NULL) {
        goto end;
    }

    line_number += 1;
    idx = 0;
    out_idx = 0;

parse_one_token:
    if (input_buffer[idx] == '\0') {
        goto next_line;
    }
    
    switch (parse_mode) {
    case PARSE_NORMAL:
        goto parse_normal_token;
    case PARSE_COMMENT:
        goto parse_comment;
    }

parse_normal_token:
    switch (input_buffer[idx]) {
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
    case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
    case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
    case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
    case 'Y': case 'Z': 
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
    case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
    case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x':
    case 'y': case 'z':
    case '_': case '$':
        goto parse_id;
    
    case '1': case '2': case '3': case '4': case '5': case '6':
    case '7': case '8': case '9': case '0':
        goto parse_number;
    
    case '\'':
        goto parse_char;
    
    case '"':
        goto parse_string;
    
    case '/':
        goto maybe_parse_comment;
    
    default:
        output_buffer[out_idx++] = input_buffer[idx++];
        goto parse_one_token;
    }

parse_id:
    memset(token_buffer, 0, 512);
    token_buf_idx = 0;
    while (isalnum((int)input_buffer[idx])
           || input_buffer[idx] == '_'
           || input_buffer[idx] == '$') {
        token_buffer[token_buf_idx++] = input_buffer[idx++];
    }

    if (!strcmp(token_buffer, "CCTY")) {
        goto parse_id_rename_type;
    } else if (!strcmp(token_buffer, "CCFN")) {
        goto parse_id_rename_fn;
    } else {
        out_idx += token_buf_idx;
        strcat(output_buffer, token_buffer);
    }
    goto parse_one_token;

parse_id_rename_type:
    if (input_buffer[idx] != '(') {
        error_reason = "expected \'(\' after proc macro name";
        goto error;
    }
    idx++;
    memset(token_buffer, 0, 512);
    token_buf_idx = 0;

    while (input_buffer[idx] != ')' && input_buffer[idx] != '\0') {
        token_buffer[token_buf_idx++] = input_buffer[idx++];
    }
    if (input_buffer[idx] != ')') {
        error_reason = "expected \')\' after proc macro parameters";
    }
    idx++;
    
    memset(rename_buffer, 0, 512);
    CCTY_RENAME(rename_buffer, token_buffer);
    strcat(output_buffer, rename_buffer);
    out_idx += strlen(rename_buffer);
    goto parse_one_token;

parse_id_rename_fn:
    if (input_buffer[idx] != '(') {
        error_reason = "expected \'(\' after proc macro name";
        goto error;
    }
    idx++;
    memset(token_buffer, 0, 512);
    token_buf_idx = 0;

    while (input_buffer[idx] != ')' && input_buffer[idx] != '\0') {
        token_buffer[token_buf_idx++] = input_buffer[idx++];
    }
    if (input_buffer[idx] != ')') {
        error_reason = "expected \')\' after proc macro parameters";
        goto error;
    }
    idx++;
    
    memset(rename_buffer, 0, 512);
    CCFN_RENAME(rename_buffer, token_buffer);
    strcat(output_buffer, rename_buffer);
    out_idx += strlen(rename_buffer);
    goto parse_one_token;

parse_char:
    output_buffer[out_idx++] = input_buffer[idx++];
    if (input_buffer[idx] == '\\') {
        output_buffer[out_idx++] = input_buffer[idx++];
    }
    output_buffer[out_idx++] = input_buffer[idx++];
    if (input_buffer[idx] != '\'') {
        error_reason = "unclosed character literal";
        goto error;
    }
    output_buffer[out_idx++] = input_buffer[idx++];
    goto parse_one_token;

parse_string:
    output_buffer[out_idx++] = input_buffer[idx++];
    while (input_buffer[idx] != '"' && input_buffer[idx] != '\0') {
        if (input_buffer[idx] == '\\') {
            output_buffer[out_idx++] = input_buffer[idx++];
        }
        output_buffer[out_idx++] = input_buffer[idx++];
    }
    if (input_buffer[idx] != '"') {
        error_reason = "unclosed string literal";
        goto error;
    }
    output_buffer[out_idx++] = input_buffer[idx++];
    goto parse_one_token;

parse_number:
    while ((int)isdigit(input_buffer[idx])) {
        output_buffer[out_idx++] = input_buffer[idx++];
    }
    goto parse_one_token;

maybe_parse_comment:
    output_buffer[out_idx++] = input_buffer[idx++];
    if (input_buffer[idx] == '*') {
        output_buffer[out_idx++] = input_buffer[idx++];
        parse_mode = PARSE_COMMENT;
        goto parse_comment;
    }
    goto parse_one_token;
    
parse_comment:
    while (input_buffer[idx] != '\0') {
        if (input_buffer[idx] == '*' && input_buffer[idx + 1] == '/') {
            output_buffer[out_idx++] = input_buffer[idx++];
            output_buffer[out_idx++] = input_buffer[idx++];
            parse_mode = PARSE_NORMAL;
            break;
        }
        output_buffer[out_idx++] = input_buffer[idx++];
    }
    goto parse_one_token;

next_line:
    output_buffer[out_idx] = '\0';
    fprintf(fp_out, "%s", output_buffer);
    memset(output_buffer, 0, 4096);
    goto line_feed;

error:
    ret_value = -1;
    if (error_reason != NULL) {
        fprintf(stderr, "cc_proc_macro: file %s: line %d, col %d: %s\n",
                argv[1], line_number, idx, error_reason);
    }
    /* fallthrough */

end:
    fclose(fp_in);
    fclose(fp_out);
    return ret_value;
}
