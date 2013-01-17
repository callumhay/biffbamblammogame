#ifndef __TRANSFORMHELPER_H__
#define __TRANSFORMHELPER_H__

#include "BasicIncludes.h"

class TransformHelper {
public:
    template <typename T> static void TurnIntoRelativeTranslations(std::vector<T>& values);


private:
    TransformHelper() {}
    ~TransformHelper() {}

    DISALLOW_COPY_AND_ASSIGN(TransformHelper);
};

template <typename T> void TransformHelper::TurnIntoRelativeTranslations(std::vector<T>& values) {
    if (values.empty()) {
        return;
    }
    
    T prevValue = values.front();
    T currValue;

    for (int i = 1; i < static_cast<int>(values.size()); i++) {
        currValue = values[i];
        values[i] = -prevValue + currValue;
        prevValue = currValue;
    }
}

#endif // __TRANSFORMHELPER_H__