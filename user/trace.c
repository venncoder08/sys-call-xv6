#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(2, "Usage: trace mask command [args...]\n");
        exit(1);
    }

    int mask = atoi(argv[1]); // Chuyển số trace mask từ chuỗi sang số nguyên

    // Gọi syscall trace để bật tracing
    if (trace(mask) < 0) {
        fprintf(2, "trace: failed to enable tracing\n");
        exit(1);
    }

    // Chạy lệnh tiếp theo (ví dụ: grep hello README)
    exec(argv[2], &argv[2]);
    
    // Nếu exec thất bại
    fprintf(2, "trace: exec failed\n");
    exit(1);
}

