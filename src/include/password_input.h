void password_input(struct nk_context* ctx, int flags, char* buffer, int len, nk_plugin_filter filter) {
    nk_size password_begin = ctx->current->buffer.end;

    // Perform the password input using Nuklear
    nk_edit_string_zero_terminated(ctx, flags, buffer, len, filter);

    // Handle password input by replacing characters with '*'
    const nk_size last = ctx->current->buffer.last;
    const nk_byte* buf = (nk_byte*)ctx->memory.memory.ptr;

    for (nk_size i = password_begin; i != last; i = ((struct nk_command*)(buf + i))->next) {
        struct nk_command* it = (struct nk_command*)(buf + i);
        if (it->type == NK_COMMAND_TEXT) {
            struct nk_command_text* text_cmd = (struct nk_command_text*)it;
            for (int j = 0; j < text_cmd->length; ++j) {
                text_cmd->string[j] = '*';
            }
        }
    }
}
