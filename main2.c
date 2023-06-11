#include <stdio.h>

enum version {
    version_init,
    version_unit_v2,
    version_count, // keep last.
};

struct unit {
    struct {
        float x;
        float y;
    } v1;
    struct {
        int damage;
        int hp;
        int mp;
    } v2;
};

struct game {
    enum version version;
    unsigned int units_count;
    struct unit units[8];
};

void encode(struct game *src)
{
    FILE *file = fopen("saved2", "wb+");
    if (!file) {
        printf("failed to open file.\n");
        return;
    }
    enum version last_version = version_count - 1;
    
    // save last version.
    fwrite(&last_version, sizeof(last_version), 1, file);
    
    // here it's up to you how
    // you decide to save the fields/props.
    // in my case, i want to save first
    // the amount of units and then, the
    // units.
    fwrite(&src->units_count, sizeof(src->units_count), 1, file);
    fwrite(src->units, sizeof(src->units), src->units_count, file);
    
    fclose(file);
    printf("saved.\n");
}

void decode(struct game *dest)
{
#define restore(field, inclusion_version) \
if (dest->version >= (inclusion_version)) \
fread(&dest->field, sizeof(dest->field), 1, file)
    
    FILE *file = fopen("saved2", "rb+");
    if (!file) {
        printf("failed to open file.\n");
        return;
    }
    
    // restore version.
    restore(version, version_init);
    
    // then restore based on how you decided
    // to save the fields/props.
    // again, in my case, first i need to get
    // the amount of units saved and then,
    // the units.
    restore(units_count, version_init);
    for (unsigned int i = 0; i < dest->units_count; i++) {
        restore(units[i].v1, version_init);
        restore(units[i].v2, version_unit_v2);
    }
    
    fclose(file);
    printf("restored.\n");
}

int main(int argc, char **argv)
{
    struct game game = {0};
    
    int do_save = argc > 1;
    if (do_save) {
        game.units[0].v1.x = 42.0f;
        game.units[0].v1.y = 24.0f;
        game.units[0].v2.damage = 2;
        game.units[0].v2.hp = 10;
        game.units[0].v2.mp = 20;
        game.units[1].v1.x = -43.0f;
        game.units[1].v1.y = 9.0f;
        game.units[1].v2.damage = 4;
        game.units[1].v2.hp = 4;
        game.units[1].v2.mp = 40;
        game.units_count = 2;
        encode(&game);
    } else {
        decode(&game);
        printf("version is %d. "
               "there are %d units saved. "
               "\n"
               "units[0].x %f\n"
               "units[0].y %f\n"
               "units[0].damage %d\n"
               "units[0].hp %d\n"
               "units[0].mp %d\n"
               "units[1].x %f\n"
               "units[1].y %f\n"
               "units[1].damage %d\n"
               "units[1].hp %d\n"
               "units[1].mp %d\n"
               ,
               game.version,
               game.units_count,
               game.units[0].v1.x,
               game.units[0].v1.y,
               game.units[0].v2.damage,
               game.units[0].v2.hp,
               game.units[0].v2.mp,
               game.units[1].v1.x,
               game.units[1].v1.y,
               game.units[1].v2.damage,
               game.units[1].v2.hp,
               game.units[1].v2.mp);
    }
    
    return 0;
}
