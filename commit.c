#include "commit.h"
#include "tree.h"
#include "index.h"
#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------- CREATE COMMIT ----------------
int commit_create(const char *message, ObjectID *id_out) {
    Index idx;

    if (index_load(&idx) != 0) return -1;

    ObjectID tree_id;
    if (tree_from_index(&tree_id) != 0) return -1;

    char tree_hex[HASH_HEX_SIZE + 1];
    hash_to_hex(&tree_id, tree_hex);

    const char *author = getenv("PES_AUTHOR");
    if (!author) author = "PES User <pes@localhost>";

    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
             "tree %s\n"
             "author %s\n"
             "message %s\n",
             tree_hex, author, message);

    if (object_write(OBJ_COMMIT, buffer, strlen(buffer), id_out) != 0)
        return -1;

    return 0;
}

// ---------------- WALK COMMITS ----------------
int commit_walk(commit_walk_fn callback, void *ctx) {
    // Simple implementation: no history tracking yet
    // Just return success (log command won’t crash)

    (void)callback;
    (void)ctx;

    return 0;
}
