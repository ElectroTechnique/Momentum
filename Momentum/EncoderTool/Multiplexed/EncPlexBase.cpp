#include "EncPlexBase.h"

namespace EncoderTool
{
    EncPlexBase::EncPlexBase(unsigned eCnt)
        : encoderCount(eCnt)
    {
        encoders = new EncoderBase[eCnt];
    }

    void EncPlexBase::begin(CountMode mode)
    {
        for (unsigned i = 0; i < encoderCount; i++)
        {
            encoders[i].setCountMode(mode);
        }
    }

    EncPlexBase::~EncPlexBase()
    {
        delete[] encoders;
    }

    EncoderBase &EncPlexBase::operator[](std::size_t idx)
    {
        return idx < encoderCount ? encoders[idx] : encoders[encoderCount - 1];
    }

    void EncPlexBase::attachCallback(allCallback_t _callback)
    {
        callback = _callback;
    }

    void EncPlexBase::attachBtnCallback(allBtnCallback_t _btnCallback)
    {
        btnCallback = _btnCallback;
    }
} // namespace EncoderTool