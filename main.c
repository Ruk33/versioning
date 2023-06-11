#include <stdio.h>

enum version {
    version_init,
    version_baz,
    version_baz_removed,
    version_lorem,
    
    // don't remove and keep at last.
    version_count,
};

struct baz {
    char buf[8];
};

struct foo {
    enum version version;
    int bar;
    // struct baz baz;
    float lorem;
};

void encode(struct foo *src)
{
    FILE *file = fopen("saved", "wb+");
    if (!file) {
        printf("failed to open file.\n");
        return;
    }
    enum version last_version = version_count - 1;
    fwrite(&last_version, sizeof(last_version), 1, file);
    fwrite(&src->bar, sizeof(src->bar), 1, file);
    // fwrite(&src->baz, sizeof(src->baz), 1, file);
    fwrite(&src->lorem, sizeof(src->lorem), 1, file);
    fclose(file);
    printf("saved.\n");
}

void decode(struct foo *dest)
{
#define restore(field, inclusion_version) \
if (dest->version >= (inclusion_version)) \
fread(&dest->field, sizeof(dest->field), 1, file)
#define ignore(type, field, inclusion_version, removed_version) \
do { \
type field; \
if (dest->version >= (inclusion_version) && dest->version < (removed_version)) \
fread(&field, sizeof(field), 1, file); \
} while (0)
    
    FILE *file = fopen("saved", "rb+");
    if (!file) {
        printf("failed to open file.\n");
        return;
    }
    fread(&dest->version, sizeof(dest->version), 1, file);
    
    restore(bar, version_init);
    // restore(baz, version_baz);
    ignore(struct baz, baz, version_baz, version_baz_removed);
    restore(lorem, version_lorem);
    
    fclose(file);
    printf("restored.\n");
}

int main(int argc, char **argv)
{
    struct foo foo = {0};
    
    int do_save = argc > 1;
    if (do_save) {
        foo.bar = 23;
        // foo.baz = (struct baz) {"some"};
        foo.lorem = 2.23;
        encode(&foo);
    } else {
        decode(&foo);
        printf("version is %d. "
               "bar is %d, "
               "baz is %s "
               "lorem is %f "
               "\n", 
               foo.version, 
               foo.bar, 
               /* foo.baz.buf */ "",
               foo.lorem);
    }
    
    return 0;
}