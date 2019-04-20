#include "Cuts.h"

Cuts::Cuts(Event* event) {
  m_event = event;
}

Cuts::~Cuts() {
  
}

bool Cuts::IsInteraction() {
  return (m_event->NInteractions() == 1);
}
