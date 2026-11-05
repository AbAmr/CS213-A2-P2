#include <iostream>
#include <iomanip>
#include "RegisterFile.h"

int main() {
    // 1. إنشاء أوبجكت من مسجلات المعالج
    RegisterFile rf;
    std::cout << "--- 1. RegisterFile Initialized & Reset ---\n";
    rf.dump(); // المفروض تطبع كل المسجلات أصفار R0 إلى RF

    // 2. اختبار الكتابة والقراءة الفردية
    std::cout << "\n--- 2. Testing Write and Read ---\n";
    rf.write(0x01, 0x42); // نكتب القيمة 42 في المسجل الأول R1
    rf.write(0x05, 0xFF); // نكتب القيمة FF في المسجل R5

    uint8_t val1 = rf.read(0x01);
    uint8_t val5 = rf.read(0x05);

    std::cout << "Read from R1 (Expected 42): 0x" << std::hex << std::uppercase << (int)val1 << "\n";
    std::cout << "Read from R5 (Expected FF): 0x" << std::hex << std::uppercase << (int)val5 << "\n";

    // 3. اختبار التأكد من صحة مؤشر المسجل (isValidRegister)
    std::cout << "\n--- 3. Testing isValidRegister ---\n";
    if (rf.isValidRegister(0x0F)) {
        std::cout << "Register 0x0F is Valid (Expected)\n";
    }
    if (!rf.isValidRegister(0x15)) { // 15 أكبر من مدي الـ 16 مسجل (0-15)
        std::cout << "Register 0x15 is Invalid (Expected)\n";
    }

    // 4. عرض محتويات المسجلات بعد التعديل (Dump)
    std::cout << "\n--- 4. Dump After Writing Values ---\n";
    rf.dump();

    // 5. اختبار تصفير المسجلات (Reset)
    std::cout << "\n--- 5. Testing Reset ---\n";
    rf.reset();
    rf.dump(); // المفروض ترجع كلها أصفار تاني

    return 0;
}