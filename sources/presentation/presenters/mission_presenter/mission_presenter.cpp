#include "mission_presenter.h"

// Qt
#include <QVariant>
#include <QDebug>

// Internal
#include "domain_entry.h"

#include "mission_service.h"
#include "vehicle_service.h"
#include "command_service.h"

#include "mission.h"
#include "mission_item.h"
#include "mission_assignment.h"
#include "vehicle_description.h"

#include "mission_item_presenter.h"
#include "location_map_presenter.h"

using namespace presentation;

class MissionPresenter::Impl
{
public:
    domain::MissionService* missionService;
    domain::VehicleService* vehicleService;
    domain::CommandService* commandService;

    db::MissionPtr selectedMission;

    MissionItemPresenter* item;
    AbstractMapPresenter* map;
};

using namespace presentation;

MissionPresenter::MissionPresenter(domain::DomainEntry* entry,
                                   QObject* object):
    BasePresenter(object),
    d(new Impl())
{
    d->missionService = entry->missionService();
    d->vehicleService = entry->vehicleService();
    d->commandService = entry->commandService();

    connect(d->missionService, &domain::MissionService::missionAdded,
            this, &MissionPresenter::updateMissions);
    connect(d->missionService, &domain::MissionService::missionRemoved,
            this, &MissionPresenter::updateMissions);

    connect(d->vehicleService, &domain::VehicleService::vehicleAdded,
            this, &MissionPresenter::updateVehicles);
    connect(d->vehicleService, &domain::VehicleService::vehicleRemoved,
            this, &MissionPresenter::updateVehicles);

    d->item = new MissionItemPresenter(d->missionService, this);
    d->map = new LocationMapPresenter(d->missionService, this);
}

MissionPresenter::~MissionPresenter()
{}

void MissionPresenter::connectView(QObject* view)
{
    d->item->setView(view->findChild<QObject*>(NAME(missionItem)));
    d->map->setView(view->findChild<QObject*>(NAME(map)));

    this->updateVehicles();
    this->updateMissions();
}

void MissionPresenter::setViewConnected(bool connected)
{
    if (connected)
    {
        connect(this->view(), SIGNAL(selectMission(QString)),
                this, SLOT(onSelectMission(QString)));
        connect(this->view(), SIGNAL(addMission()),
                this, SLOT(onAddMission()));
        connect(this->view(), SIGNAL(removeMission()),
                this, SLOT(onRemoveMission()));
        connect(this->view(), SIGNAL(renameMission(QString)),
                this, SLOT(onRenameMission(QString)));
        connect(this->view(), SIGNAL(assignVehicle(QString)),
                this, SLOT(onAssignVehicle(QString)));
        connect(this->view(), SIGNAL(uploadMission()),
                this, SLOT(onUploadMission()));
        connect(this->view(), SIGNAL(downloadMission()),
                this, SLOT(onDownloadMission()));
    }
    else
    {
        disconnect(this->view(), 0, this, 0);
    }
}

void MissionPresenter::updateMissions()
{
    this->setViewConnected(false);

    QStringList missions;
    missions.append(QString());

    for (const db::MissionPtr& mission: d->missionService->missions())
    {
        missions.append(mission->name());
    }
    this->setViewProperty(PROPERTY(missions), QVariant::fromValue(missions));
    this->setViewConnected(true);
}

void MissionPresenter::updateAssignment()
{
    this->setViewConnected(false);

    if (d->selectedMission)
    {
        db::MissionAssignmentPtr assignment =
                d->missionService->missionAssignment(d->selectedMission);
        if (assignment)
        {
            this->setViewProperty(PROPERTY(assignedStatus), assignment->status());

            db::VehicleDescriptionPtr vehicle =
                    d->vehicleService->description(assignment->vehicleId());
            if (vehicle)
            {
                this->setViewProperty(PROPERTY(assignedVehicle),
                                      vehicle->name());
            }
            this->setViewConnected(true);
            return;
        }
    }
    this->setViewProperty(PROPERTY(assignedVehicle), QString());
    this->setViewProperty(PROPERTY(assignedStatus), db::MissionAssignment::Unknown);

    this->setViewConnected(true);
}

void MissionPresenter::updateVehicles()
{
    this->setViewConnected(false);

    QStringList vehicles;
    vehicles.append(QString());

    for (const db::VehicleDescriptionPtr& vehicle: d->vehicleService->descriptions())
    {
        vehicles.append(vehicle->name());
    }
    this->setViewProperty(PROPERTY(vehicles), QVariant::fromValue(vehicles));
    this->setViewConnected(true);

    this->updateAssignment();
}

void MissionPresenter::onSelectMission(const QString& name)
{
    d->selectedMission = name.isEmpty() ?
                             db::MissionPtr() :
                             d->missionService->missionByName(name);

    this->updateAssignment();
    d->item->setMission(d->selectedMission);
}

void MissionPresenter::onAddMission()
{
    db::MissionPtr mission = db::MissionPtr::create();

    mission->setName(tr("New Mission %1").arg(
                         d->missionService->missions().count()));

    d->missionService->saveMission(mission);
    this->setViewProperty(PROPERTY(selectedMission), mission->name());
}

void MissionPresenter::onRemoveMission()
{
    if (d->selectedMission.isNull()) return;

    d->missionService->removeMission(d->selectedMission);
    d->selectedMission.clear();
    this->setViewProperty(PROPERTY(selectedMission), QString());

    d->item->setMission(d->selectedMission);
}

void MissionPresenter::onRenameMission(const QString& name)
{
    if (d->selectedMission.isNull() || name.isEmpty()) return;

    // TODO: check unique name
    d->selectedMission->setName(name);
    d->missionService->saveMission(d->selectedMission);
    this->updateMissions();
    this->setViewProperty(PROPERTY(selectedMission), name);
}

void MissionPresenter::onAssignVehicle(const QString& name)
{
    if (!d->selectedMission) return;
    db::VehicleDescriptionPtr vehicle =
            d->vehicleService->findDescriptionByName(name);

    if (vehicle)
    {
        d->missionService->assign(d->selectedMission, vehicle);
    }
    else
    {
        d->missionService->unassign(d->selectedMission);
    }

    this->updateAssignment();
}

void MissionPresenter::onUploadMission()
{
    if (!d->selectedMission) return;
    db::MissionAssignmentPtr assignment =
            d->missionService->missionAssignment(d->selectedMission);
    if (assignment.isNull()) return;

    d->commandService->upload(assignment);
    this->updateAssignment();
}

void MissionPresenter::onDownloadMission()
{
    if (!d->selectedMission) return;
    db::MissionAssignmentPtr assignment =
            d->missionService->missionAssignment(d->selectedMission);
    if (assignment.isNull()) return;

    d->commandService->download(assignment);
    this->updateAssignment();
}
