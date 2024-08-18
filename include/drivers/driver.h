#ifndef __HYDRAOS__DRIVERS__DRIVER_H
#define __HYDRAOS__DRIVERS__DRIVER_H

namespace hydraos {
    namespace drivers {
        class Driver {
            public:
                Driver();
                ~Driver();
                virtual void Activate();
                virtual int Reset();
                virtual void Deactivate();
        };

        class DriverManager {
            public:
                DriverManager();
                void AddDriver(Driver*);
                void ActivateAll();
            private:
                Driver* drivers[255];
                int numDrivers;
        };
    }
}

#endif