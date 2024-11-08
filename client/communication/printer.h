//
// Created by fran on 24/10/24.
//

#ifndef TALLER_TP_PRINTER_H
#define TALLER_TP_PRINTER_H

#include "common/events/base/event.h"
#include "common/queue.h"
#include "common/thread.h"
#include <memory>

class Printer : public Thread {
private:
  Queue<std::shared_ptr<Event>> *events;

public:
  explicit Printer(Queue<std::shared_ptr<Event>> *events);

  void run() override;

  void process_event(std::shared_ptr<Event> &event) const;
};

#endif // TALLER_TP_PRINTER_H