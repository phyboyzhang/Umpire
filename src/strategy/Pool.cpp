#include "umpire/strategy/Pool.hpp"
#include "umpire/util/Macros.hpp"

#include "umpire/util/AllocatorTraits.hpp"

namespace umpire {
namespace strategy {

Pool::Pool(
    const std::string& name,
    int id,
    util::AllocatorTraits traits,
    std::vector<std::shared_ptr<AllocationStrategy> > providers) :
  AllocationStrategy(name, id),
  m_current_size(0),
  m_highwatermark(0)
{
  m_slots = traits.m_number_allocations;

  m_allocator = providers[0];


  UMPIRE_LOG(Debug, "Creating " << m_slots << "-slot pool.");

  m_lengths = new size_t[m_slots];
  m_pointers = new void*[m_slots];

  for (size_t i = 0; i < m_slots; ++i) {
    m_pointers[i] = nullptr;
    m_lengths[i] = 0;
  }
}

void*
Pool::allocate(size_t bytes)
{
  void* ptr = nullptr;

  for (size_t i = 0; i < m_slots; ++i) {
     if (m_lengths[i] == bytes) {
        m_lengths[i] = -m_lengths[i] ;
        ptr = m_pointers[i] ;
        break ;
     } else if (m_lengths[i] == 0) {
        m_lengths[i] = -static_cast<int>(bytes) ;
        m_pointers[i] = m_allocator->allocate(bytes);
        ptr = m_pointers[i] ;
        break ;
     }
  }

  UMPIRE_LOG(Debug, "(bytes=" << bytes << ") returning " << ptr);
  return ptr;
}

void
Pool::deallocate(void* ptr)
{
  UMPIRE_LOG(Debug, "(ptr=" << ptr << ")");
  for (size_t i = 0; i < m_slots; ++i) {
    if (m_pointers[i] == ptr) {
      m_lengths[i] = -m_lengths[i];
      ptr = nullptr;
      break;
    }
  }
}

long 
Pool::getCurrentSize()
{
  UMPIRE_LOG(Debug, "() returning " << m_current_size);
  return m_current_size;
}

long 
Pool::getHighWatermark()
{
  UMPIRE_LOG(Debug, "() returning " << m_highwatermark);
  return m_highwatermark;
}

Platform
Pool::getPlatform()
{
  return m_allocator->getPlatform();
}


} // end of namespace alloc
} // end of namespace pool