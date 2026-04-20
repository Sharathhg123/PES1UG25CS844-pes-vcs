#include "commit.h"
#include "tree.h"
#include "index.h"
#include "pes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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

    // write commit object
    if (object_write(OBJ_COMMIT, buffer, strlen(buffer), id_out) != 0)
        return -1;

    // convert commit hash to hex
    char hex[HASH_HEX_SIZE + 1];
    hash_to_hex(id_out, hex);

    // create refs directories
    mkdir(".pes/refs", 0755);
    mkdir(".pes/refs/heads", 0755);

    // write main branch
    FILE *f = fopen(".pes/refs/heads/main", "w");
    if (f) {
        fprintf(f, "%s\n", hex);
        fclose(f);
    }

    // write HEAD pointer
    f = fopen(".pes/HEAD", "w");
    if (f) {
        fprintf(f, "ref: refs/heads/main\n");
        fclose(f);
    }

    return 0;
}

// ---------------- WALK (LOG) ----------------
int commit_walk(commit_walk_fn callback, void *ctx) {
    (void)callback;
    (void)ctx;

    FILE *f = fopen(".pes/refs/heads/main", "r");
    if (!f) {
        printf("No commits yet\n");
        return -1;
    }

    char hex[HASH_HEX_SIZE + 1];
    fscanf(f, "%s", hex);
    fclose(f);

    ObjectID id;
    hex_to_hash(hex, &id);

    ObjectType type;
    void *data;
    size_t len;

    if (object_read(&id, &type, &data, &len) != 0) {
        printf("Error reading commit\n");
        return -1;
    }

    printf("Commit: %s\n", hex);
    printf("%.*s\n", (int)len, (char *)data);

    free(data);
    return 0;
}
