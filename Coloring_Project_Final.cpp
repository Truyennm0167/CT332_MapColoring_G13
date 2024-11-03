#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REGIONS 100 // Giới hạn số vùng tối đa
#define NO_COLOR 0      // Không có màu
#define RED 1           // Màu đỏ
#define BLUE 2          // Màu xanh dương
#define YELLOW 3        // Màu vàng

// Cấu trúc lưu trữ thông tin về vùng và màu sắc
typedef struct {
    int adjacents[MAX_REGIONS][MAX_REGIONS]; 	// Mảng lưu các vùng kề nhau
    int num_adjacents[MAX_REGIONS];           	// Số vùng kề cho từng vùng
    int num_regions;                           	// Tổng số vùng
    int colors[MAX_REGIONS];                   	// Mảng lưu màu cho từng vùng
} RegionColoring;

// Khởi tạo bản đồ với số vùng cho trước
void initializeMap(RegionColoring* map, int num_regions) {
    map->num_regions = num_regions; 			// Gán số vùng
    for (int i = 0; i < num_regions; i++) {
        map->num_adjacents[i] = 0; 				// Đặt số vùng kề là 0
        map->colors[i] = NO_COLOR; 				// Đặt màu ban đầu là không có màu
    }
}

// Thêm vùng kề vào vùng hiện tại
void addAdjacent(RegionColoring* map, int region, int adjacent) {
    int index = map->num_adjacents[region];		// Lấy vị trí để thêm vùng kề mới
	map->adjacents[region][index] = adjacent; 	// Thêm vùng kề
	map->num_adjacents[region]++;				// Tăng số lượng vùng kề lên
}

// Trả về tên màu tương ứng với mã màu
const char* getColorName(int color) {
    switch (color) {
        case RED: return "\033[31mMau do\033[0m"; 				// Nếu màu đỏ
        case BLUE: return "\033[34mMau xanh duong\033[0m"; 		// Nếu màu xanh dương
        case YELLOW: return "\033[33mMau vang\033[0m"; 			// Nếu màu vàng
        default: return "NO COLOR"; 							// Nếu không có màu
    }
}

// Hiển thị màu sắc của các vùng
void displayMap(const RegionColoring* map) {
    for (int i = 0; i < map->num_regions; i++) {
        printf("Vung %d: %s\n", i+1, getColorName(map->colors[i])); // In vùng và màu tương ứng
    }
}

// Kiểm tra xem màu có hợp lệ cho vùng hiện tại hay không
int isColorValid(const RegionColoring* map, int region, int color) {
    for (int i = 0; i < map->num_adjacents[region]; i++) {
        int adjacent = map->adjacents[region][i]; 	// Duyệt qua các hàng xóm của bang hiện tại
        if (map->colors[adjacent] == color) {
            return 0; 	// Không hợp lệ nếu vùng kề có cùng màu
        }
    }
    return 1; 			// Hợp lệ
}

// Hàm đệ quy để tô màu vùng
int backtrack(RegionColoring* map, int region) {
    if (region == map->num_regions) {
        return 1; 		// Tất cả các vùng đã được tô màu
    }

    // Thử các màu cho vùng hiện tại
    for (int color = RED; color <= YELLOW; color++) {
        if (isColorValid(map, region, color)) {
            map->colors[region] = color; 	// Tô màu cho vùng hiện tại

            // Gọi đệ quy để tô màu vùng tiếp theo
            if (backtrack(map, region + 1)) {
                return 1; 					// Nếu thành công, trả về 1
            }

            // Khôi phục màu nếu không thành công
            map->colors[region] = NO_COLOR;
        }
    }

    return 0; 								// Không tìm được giải pháp, trả về 0
}

// Đếm số dòng trong tệp
int countLinesInFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Khong the mo file %s\n", filename);
        return -1;
    }
    int count = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            count++; 	// Tăng biến đếm mỗi khi gặp ký tự xuống dòng
        }
    }
    fclose(file);
    return count; 		// Trả về số dòng đếm được
}

// Đọc bản đồ từ tệp
void readMapFromFile(RegionColoring* map, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file"); 			// Thông báo lỗi nếu không mở được tệp
        exit(1);
    }
    
    char buffer[256];
    int region = 0;

    // Đọc từng dòng từ file
    while (fgets(buffer, sizeof(buffer), file)) {
        char* token = strtok(buffer, " ");
        while (token != NULL) {
            int adjacent = atoi(token) - 1; 	// Giảm 1 để chỉ số bắt đầu từ 0
            addAdjacent(map, region, adjacent); // Thêm vùng kề
            token = strtok(NULL, " ");
        }
        region++; 				// Tăng chỉ số vùng sau mỗi dòng
    }
    map->num_regions = region; 	// Cập nhật số lượng vùng
    fclose(file); 				// Đóng tệp
}

int main() {
    const char* filename = "map.txt"; 					// Tên tệp chứa bản đồ
    int lines_count = countLinesInFile(filename); 		// Đếm số dòng trong tệp
    RegionColoring map;
    initializeMap(&map, lines_count); 					// Khởi tạo bản đồ với số vùng
    readMapFromFile(&map, filename); 					// Đọc bản đồ từ tệp
    printf("So vung can to mau: %d\n\n", lines_count); 	// In tổng số vùng của bản đồ

    // Thực hiện tô màu với backtracking
    if (backtrack(&map, 0)) {
        printf("Da tim duoc giai phap!\n\n");			// In thông báo, nếu tìm thấy giải pháp
        printf("Giai phap vua tim duoc la:\n");
		displayMap(&map); 								// Hiển thị kết quả tô màu
    } else {
        printf("Khong tim duoc giai phap.\n"); 			// In thông báo, nếu KHÔNG tìm thấy giải pháp
    }

    return 0; 											// Kết thúc chương trình
}
