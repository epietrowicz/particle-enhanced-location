#include "Particle.h"
#include "QuectelTowerRK.h"

SYSTEM_MODE(AUTOMATIC);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

const std::chrono::milliseconds publishPeriod = 60s;
unsigned long lastPublish;
CloudEvent event;

void publishLoc();

void locEnhancedEventHandler(const char *name, const char *data)
{
  Log.info("location enhanced event: %s data: %s", name, data ? data : "NULL");
}

void setup()
{
  Particle.subscribe("loc-enhanced", locEnhancedEventHandler);
}

void loop()
{
  if (Particle.connected())
  {
    if ((lastPublish == 0) || (millis() - lastPublish >= publishPeriod.count()))
    {
      lastPublish = millis();
      publishLoc();
    }
  }
}

void publishLoc()
{
  if (!Cellular.ready())
  {
    Log.error("Cellular is not ready");
    return;
  }

  QuectelTowerRK::TowerInfo towerInfo;
  Variant towers;
  int res = QuectelTowerRK::instance().scanBlocking(towerInfo);
  if (res != SYSTEM_ERROR_NONE)
  {
    Log.error("Failed to obtain CGI: res=%d", res);
    return;
  }
  towerInfo.log("towerInfo", LOG_LEVEL_INFO);
  towerInfo.toVariant(towers);

  Variant obj;
  obj.set("cmd", "loc");

  Variant loc;
  loc.set("lck", 0);
  loc.set("time", Time.now());
  obj.set("loc", loc);
  obj.set("towers", towers);

  // Set up the publish event
  event.name("loc");
  event.data(obj);
  Particle.publish(event);

  Log.info("publishing %s", obj.toJSON().c_str());

  // Wait while sending (blocking)
  waitForNot(event.isSending, 60000);

  if (!event.isOk())
  {
    Log.error("publish failed error=%d", event.error());
    return;
  }
  Log.info("publish succeeded");
  event.clear();
}
