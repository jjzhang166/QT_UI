#ifndef _PBC_UTIL_
#define _PBC_UTIL_

#include "ByteBuffer.h"
#include "pbc.h"

static QList<QVariant> makePair(QVariant value, string strType)
{
    QList<QVariant> resultList;
    resultList << value << QString::fromLatin1(strType.c_str(), strType.size());
    return resultList;
}

static void
dump_bytes(const char *data, size_t len) {
    size_t i;
    for (i = 0; i < len; i++)
        if (i == 0)
            fprintf(stdout, "%02x", 0xff & data[i]);
        else
            fprintf(stdout, " %02x", 0xff & data[i]);
}

static void dump_message(struct pbc_rmessage *m, int level, QVariantMap& mapValue);

static void
dump_value(struct pbc_rmessage *m, const char *key, int type, int idx, int level,
           QVariantMap& mapValue, QList<QVariant>* tmpChild) {
    int i;
    for (i=0;i<level;i++) {
        printf("  ");
    }
    printf("%s",key);
    int srcType = type;
    if (type & PBC_REPEATED) {
        printf("[%d]",idx);
        type -= PBC_REPEATED;
    }
    printf(": ");

    uint32_t low;
    uint32_t hi;
    uint64_t ret64;
    double real;
    const char *str;
    int str_len;

    QVariant realVal;
    QVariantMap childMapV;

    string strType;
    switch(type) {
    case PBC_INT:
    case PBC_UINT:
        low = pbc_rmessage_integer(m, key, idx, NULL);
        printf("%d", (int) low);
        realVal = (int) low;
        strType = "int";
        break;
    case PBC_REAL:
        real = pbc_rmessage_real(m, key , idx);
        printf("%lf", real);
        realVal = real;
        strType = "real";
        break;
    case PBC_BOOL:
        low = pbc_rmessage_integer(m, key, idx, NULL);
        printf("%s", low ? "true" : "false");
        realVal = (bool)low;
        strType = "bool";
        break;
    case PBC_ENUM:
        str = pbc_rmessage_string(m, key , idx , NULL);
        printf("[%s]", str);
        realVal = str;
        strType = "enum";
        break;
    case PBC_STRING:
        str = pbc_rmessage_string(m, key , idx , NULL);
        printf("'%s'", str);
        realVal = str;
        strType = "string";
        break;
    case PBC_MESSAGE:
        printf("\n");
        dump_message(pbc_rmessage_message(m, key, idx),level+1, childMapV);
        realVal = childMapV;
        strType = "message";
        break;
    case PBC_FIXED64:
    case PBC_INT64:
        low = pbc_rmessage_integer(m, key, idx, &hi);
        printf("0x%8x%8x",hi,low);
        memcpy((char*)(&ret64), &low, sizeof(low));
        memcpy((char*)(&ret64) + 4, &hi, sizeof(hi));
        realVal = (uint64)ret64;
        strType = "int64";
        break;
    case PBC_FIXED32:
        low = pbc_rmessage_integer(m, key, idx, NULL);
        printf("0x%x",low);
        realVal = low;
        strType = "int";
        break;
    case PBC_BYTES:
        str = pbc_rmessage_string(m, key, idx, &str_len);
        dump_bytes(str, str_len);
        strType = "string";
        break;
    default:
        printf("unknown %d", type);
        break;
    }

    printf("\n");
    if (srcType & PBC_REPEATED) {
        if (tmpChild)
           *tmpChild << QVariant(makePair(realVal, strType));
    }
    else{
        mapValue[key] = makePair(realVal, strType);
    }
}

static void
dump_message(struct pbc_rmessage *m, int level, QVariantMap& mapValue) {
    int t = 0;
    const char *key = NULL;
    for (;;) {
        t = pbc_rmessage_next(m, &key);
        if (key == NULL)
            break;
        if (t & PBC_REPEATED) {
            int n = pbc_rmessage_size(m, key);
            int i;
            QList<QVariant> tmpChild;
            for (i=0;i<n;i++) {

                dump_value(m, key , t , i , level, mapValue, &tmpChild);
            }
            mapValue[key]     = makePair(tmpChild, "repeated");
        } else {
            dump_value(m, key , t , 0 , level, mapValue, NULL);
        }
    }
}
static void
read_file(const char *filename , struct pbc_slice *slice) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        fprintf(stderr, "Can't open file %s\n", filename);
        return;
    }
    fseek(f,0,SEEK_END);
    slice->len = ftell(f);
    fseek(f,0,SEEK_SET);
    slice->buffer = malloc(slice->len);
    fread(slice->buffer, 1 , slice->len , f);
    fclose(f);
}


#endif
