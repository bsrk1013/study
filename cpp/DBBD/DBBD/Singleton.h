#pragma once

namespace DBBD {
    template < typename T >
    class Singleton
    {
    protected:
        Singleton() {}
        virtual ~Singleton() {}

    public:
        static T* Instance()
        {
            if (instance == NULL) {
                instance = new T;
                atexit(destroyInstance);
            }
            return instance;
        };

        static void destroyInstance()
        {
            if (instance) {
                instance->release();
            }
        }

       /* static void DestroyInstance()
        {
            if (m_pInstance)
            {
                delete m_pInstance;
                m_pInstance = NULL;
            }
        };*/

    protected:
        virtual void release() = 0;

    private:
        static T* instance;
    };

    template <typename T> T* Singleton<T>::instance = nullptr;
}