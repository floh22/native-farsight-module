#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include "windows.h"

class WinApiException : public std::runtime_error {

public:
	WinApiException(const char* message)
		:std::runtime_error(message){

		errorCode = GetLastError();
		this->message = message;
	}

	std::string GetErrorMessage() {
		std::string msg = std::string(message);

		if (errorCode > 0) {
			char winapiError[255];
			FormatMessageA(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				errorCode,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				winapiError,
				(sizeof(winapiError) / sizeof(wchar_t)),
				NULL);

			msg.append("(");
			msg.append(winapiError);
			msg.append(")");
		}

		return msg;
	}

private:
	const char*    message = nullptr;
	int            errorCode = 0;
};

namespace Memory
{
    DWORD ReadDWORD(HANDLE hProcess, DWORD address);

    void Read(HANDLE hProcess, DWORD address, void *buffer, int size);

    DWORD ReadDWORDFromBuffer(void *buffer, int offset);
}

namespace Process
{
    BOOL IsProcessRunning(DWORD pid);
}

namespace Character
{
    bool ContainsOnlyASCII(const char *buff, int maxSize);
    std::string ToLower(std::string str);
    std::string RandomString(const int len);
    std::string Format(const char *c, const char *args...);
    bool ArrayStartsWith(const char *array, const char *start);
    bool VectorStartsWith(const std::vector<char> &array, const char *start);
}

namespace vec3
{
    inline void inverse(float v[3])
    {
        v[0] = -v[0];
        v[1] = -v[1];
        v[2] = -v[2];
    }

    inline bool cmpf(float A, float B, float epsilon = 0.005f)
    {
        return (fabs(A - B) < epsilon);
    }

    inline bool cmp(const float v1[3], const float v2[3])
    {
        //compare with epsilon
        return (
            cmpf(v1[0], v2[0]) &&
            cmpf(v1[1], v2[1]) &&
            cmpf(v1[2], v2[2]));
    }

    inline void copy(const float v[3], float out[3])
    {
        out[0] = v[0];
        out[1] = v[1];
        out[2] = v[2];
    }

    inline void translate(const float v0[3], const float v1[3], float out[3])
    {
        out[0] = v0[0] + v1[0];
        out[1] = v0[1] + v1[1];
        out[2] = v0[2] + v1[2];
    }

    inline void scale(const float v[3], float s, float out[3])
    {
        out[0] = v[0] * s;
        out[1] = v[1] * s;
        out[2] = v[2] * s;
    }

    inline void add(const float v0[3], const float v1[3], float out[3])
    {
        out[0] = v0[0] + v1[0];
        out[1] = v0[1] + v1[1];
        out[2] = v0[2] + v1[2];
    }

    inline void sub(const float v0[3], const float v1[3], float out[3])
    {
        out[0] = v0[0] - v1[0];
        out[1] = v0[1] - v1[1];
        out[2] = v0[2] - v1[2];
    }

    inline void cross(const float v0[3], const float v1[3], float out[3])
    {
        out[0] = v0[1] * v1[2] - v0[2] * v1[1];
        out[1] = v0[2] * v1[0] - v0[0] * v1[2];
        out[2] = v0[0] * v1[1] - v0[1] * v1[0];
    }

    inline float dot(const float v0[3], const float v1[3])
    {
        return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
    }

    inline float length(const float v[3])
    {
        return sqrtf(dot(v, v));
    }

    inline void normalize(const float v[3], float out[3])
    {
        float len = length(v);
        if (len > 0.0f)
        {
            scale(v, 1.0f / len, out);
        }
    }

    inline void rotate(const float v[3], const float axis[3], float angle, float out[3])
    {
        float s = sinf(angle);
        float c = cosf(angle);
        float t = 1.0f - c;

        float x = axis[0];
        float y = axis[1];
        float z = axis[2];

        float tx = t * x;
        float ty = t * y;

        float tmp[3];
        tmp[0] = (tx * x + c) * v[0] + (tx * y - s * z) * v[1] + (tx * z + s * y) * v[2];
        tmp[1] = (tx * y + s * z) * v[0] + (ty * y + c) * v[1] + (ty * z - s * x) * v[2];
        tmp[2] = (tx * z - s * y) * v[0] + (ty * z + s * x) * v[1] + (t * z * z + c) * v[2];

        copy(tmp, out);
    }

    inline float distance(const float v0[3], const float v1[3])
    {
        float tmp[3];
        sub(v0, v1, tmp);
        return length(tmp);
    }


    inline float setLength(const float v[3], float len, float out[3])
    {
        float l = length(v);
        if (l > 0.0f)
        {
            scale(v, len / l, out);
        }
        return l;
    }
}