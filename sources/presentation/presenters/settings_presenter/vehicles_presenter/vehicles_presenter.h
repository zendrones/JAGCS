#ifndef VEHICLES_PRESENTER_H
#define VEHICLES_PRESENTER_H

// Internal
#include "base_presenter.h"

#include "dao_traits.h"

namespace domain
{
    class VehicleService;
}

namespace presentation
{
    class VehiclesPresenter: public BasePresenter
    {
        Q_OBJECT

    public:
        explicit VehiclesPresenter(QObject* parent = nullptr);
        ~VehiclesPresenter() override;

    protected:
        void connectView(QObject* view) override;

    private slots:
        void onVehicleAdded(const dao::VehiclePtr& vehicle);
        void onVehicleRemoved(const dao::VehiclePtr& vehicle);
        void updateVehicles();

        void onAddVehicle();
        void onSetAutoAdd(bool add);

    private:
        class Impl;
        QScopedPointer<Impl> const d;
    };
}

#endif // VEHICLES_PRESENTER_H
