
#ifndef RELEASE
    #define NK_INCLUDE_FIXED_TYPES
    #include "SDL_render.h"
    #define NK_INCLUDE_STANDARD_IO
    #define NK_INCLUDE_STANDARD_VARARGS
    #define NK_INCLUDE_DEFAULT_ALLOCATOR
    #define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
    #define NK_INCLUDE_FONT_BAKING
    #define NK_INCLUDE_DEFAULT_FONT
    #define NK_IMPLEMENTATION
    #define NK_ZERO_COMMAND_MEMORY
    #include "list.h"
    #include <nuklear.h>
    #include <stddef.h>
    #include <stdio.h>
#endif

#include "encrypter.h"
#include "file_util.h"
#include "password_input.h"
#include <SDL.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

typedef enum { ENCRYPT, DECRYPT } Mode;

typedef struct {
    List *file_list;
    const char *password;
    const char *current_file;
    float *progress;
    Mode mode;
    bool is_finished;
} Thread_args;

struct nk_rect bounds;
struct nk_rect bounds_group;
const struct nk_input *in_group;
const struct nk_input *in;

void process_file(Thread_args *data, Node *file_node, const char *password) {
    char *file_name = get_basename(file_node->file_name);
    char *new_file_name = NULL;

    if (data->mode == ENCRYPT) {
        new_file_name = add_extension(file_name, ".enc");
        encrypt(new_file_name, file_node->file_name, password, data->progress);
        free(new_file_name);
        free(file_name);
    } else {
        char *temp = file_name;
        char *dot = strrchr(file_name, '.');
        if (dot != NULL) {
            *dot = '\0';
        }
        decrypt(file_name, file_node->file_name, password, data->progress);
        free(temp);
    }
}

void *process_files(void *arg) {
    Thread_args *data = (Thread_args *)arg;
    if(!data->file_list) goto cleanup;
    Node *tmp = NULL;
    char *current_file = NULL;

        tmp = data->file_list->head;
        char *const passsword = str_duplicate(data->password);
        data->is_finished = false;
        while (tmp) {
            *data->progress = 0;
            current_file = get_basename(tmp->file_name);
            data->current_file = current_file;
            process_file(data, tmp, passsword);
            free(current_file);
            tmp = tmp->next;
        }
        data->current_file = NULL;
        data->is_finished = true;
        free(passsword);

    cleanup:
        pthread_exit(NULL);
}

void start_encryption_thread(List *file_list, Thread_args *args) {
    static pthread_t thread;
    if (args->is_finished) {
        pthread_create(&thread, NULL, process_files, (void *)args);
    }
}

void add_item(struct nk_context *ctx, List *list) {
    Node *current = list->head;
    Node *previous = NULL;
    while (current) {
        nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 2);
        nk_layout_row_push(ctx, 0.90f);

        in = &ctx->input;
        bounds = nk_widget_bounds(ctx);
        nk_label(ctx, current->file_name,
                 NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_LEFT);

        if (nk_input_is_mouse_hovering_rect(in, bounds) &&
            nk_input_is_mouse_hovering_rect(in_group, bounds_group))
            nk_tooltip(ctx, current->file_name);

        nk_layout_row_push(ctx, 0.10f);
        if (nk_button_label(ctx, "d")) {
            remove_node(current, previous, list);
            current = list->head;
            previous = NULL;
        } else {
            previous = current;
            current = current->next;
        }

        nk_layout_row_end(ctx);
    }
}

void crypt_button(struct nk_context *ctx, const char *label, List *list,
                  Thread_args *args, const char *password, Mode mode) {
    if (nk_button_label(ctx, label)) {
        args->password = password;
        args->file_list = list;
        args->mode = mode;
        start_encryption_thread(list, args);
    }
}
void fencrypt_ui(struct nk_context *ctx, const int w, const int h, List *list) {
    static char password[33];
    static float progress;
    static int show_password;
    static char msg[] = "S";
    static Thread_args args = {.file_list = NULL,
                               .password = NULL,
                               .is_finished = true,
                               .progress = &progress,
                               .current_file = NULL};

    if (nk_begin(ctx, "Fencrypt", nk_rect(0, 0, w, h),
                 NK_WINDOW_NO_SCROLLBAR)) {
        nk_layout_row_static(ctx, (float)300, w - 40, 1);
        in_group = &ctx->input;
        bounds_group = nk_widget_bounds(ctx);

        if (nk_group_begin(ctx, "Darg & Drop File Here",
                           NK_WINDOW_BORDER | NK_WINDOW_TITLE)) {

            add_item(ctx, list);
        }

        nk_group_end(ctx);
        nk_layout_row_dynamic(ctx, 0, 1);
        if (nk_button_label(ctx, "Clear All")) {
            clear_list(list->head);
            list->head = NULL;
        }

        nk_layout_row_begin(ctx, NK_DYNAMIC, 0, 3);
        nk_layout_row_push(ctx, 0.20f);
        nk_label(ctx, "Password:", NK_TEXT_ALIGN_MIDDLE | NK_TEXT_ALIGN_LEFT);
        nk_layout_row_push(ctx, 0.70f);
        if (show_password) {
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, password, 33,
                                           NULL);
            msg[0] = 'H';
        } else {
            password_input(ctx, NK_EDIT_FIELD, password, 33, NULL);
            msg[0] = 'S';
        }
        nk_layout_row_push(ctx, 0.10f);

        if (nk_button_label(ctx, msg)) {
            show_password = !show_password;
        }

        nk_layout_row_end(ctx);
        if (args.is_finished == false) {
            nk_layout_row_dynamic(ctx, 0, 1);
            nk_labelf(ctx, NK_TEXT_ALIGN_LEFT, "%s  %.2f", args.current_file,
                      progress);
        }
        nk_layout_row_dynamic(ctx, 0, 2);
        crypt_button(ctx, "Encrypt", list, &args, password, ENCRYPT);
        crypt_button(ctx, "Decrypt", list, &args, password, DECRYPT);
    }
    nk_end(ctx);
}
