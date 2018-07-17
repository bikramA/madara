#ifndef MADARA_KNOWLEDGE_ANY_H_
#define MADARA_KNOWLEDGE_ANY_H_

/**
 * @file Any.h
 * @author David Kyle <david.kyle@shield.ai>
 *
 * This file contains the Any class
 **/

#include <memory>
#include <sstream>
#include <type_traits>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wexceptions"
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif
#include "cereal/archives/portable_binary.hpp"
#include "cereal/archives/json.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/stack.hpp"
#include "cereal/types/queue.hpp"
#include "cereal/types/deque.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/set.hpp"
#include "cereal/types/bitset.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/unordered_set.hpp"
#include "cereal/types/string.hpp"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__

#include <boost/version.hpp>
#if BOOST_VERSION > 105600 && !defined(MADARA_NO_BOOST_TYPE_INDEX)
#define MADARA_USE_BOOST_TYPE_INDEX
#endif

#ifdef MADARA_USE_BOOST_TYPE_INDEX
#include <boost/type_index.hpp>
#else
#include <typeindex>
#endif

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

#include "madara/utility/SupportTest.h"
#include "madara/MadaraExport.h"
#include "madara/utility/StdInt.h"
#include "madara/logger/GlobalLogger.h"
#include "madara/utility/IntTypes.h"
#include "madara/exceptions/BadAnyAccess.h"

namespace madara { namespace knowledge {

using madara_oarchive = cereal::PortableBinaryOutputArchive;
using madara_iarchive = cereal::PortableBinaryInputArchive;

using json_oarchive = cereal::JSONOutputArchive;
using json_iarchive = cereal::JSONInputArchive;

#ifdef MADARA_USE_BOOST_TYPE_INDEX
using type_index = boost::typeindex::type_index;
using boost::typeindex::type_id;
#else
using type_index = std::type_index;

template<typename T>
inline type_index type_id()
{
  return type_index(typeid(T));
}
#endif

/// For internal use. Holds type information for use by Any class.
struct TypeHandlers
{
  type_index tindex;

  typedef void (*destruct_fn_type)(void *);
  destruct_fn_type destruct;

  typedef void *(*clone_fn_type)(void *);
  clone_fn_type clone;

  typedef void (*save_fn_type)(madara_oarchive &, const void *);
  save_fn_type save;

  typedef void (*load_fn_type)(madara_iarchive &, void *);
  load_fn_type load;

  typedef void (*save_json_fn_type)(json_oarchive &, const void *);
  save_json_fn_type save_json;

  typedef void (*load_json_fn_type)(json_iarchive &, void *);
  load_json_fn_type load_json;
};

/// Creates a function for deleting the given type. By default, simply call
/// delete. Specialize this function to customize otherwise.
template<typename T>
constexpr auto get_type_handler_destruct(type<T>,
    overload_priority_weakest) ->
  typename std::enable_if<std::is_nothrow_destructible<T>::value,
    TypeHandlers::destruct_fn_type>::type
{
  return [](void *ptr) {
      T *t_ptr = static_cast<T *>(ptr);
      delete t_ptr;
    };
}

/// Creates a function for deleting the given type. By default, simply call
/// new with copy constructor. Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::clone_fn_type get_type_handler_clone(type<T>,
    overload_priority_weakest)
{
  return [](void *ptr) -> void * {
      T *t_ptr = static_cast<T *>(ptr);
      return new T(*t_ptr);
    };
}

/// Creates a function for serializing the given type to a madara_oarchive.
/// By default, simply use the Boost.Serialization << operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::save_fn_type get_type_handler_save(type<T>,
    overload_priority_weakest)
{
  return [](madara_oarchive &archive, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      archive << val;
    };
}

/// Creates a function for unserializing the given type to a madara_iarchive.
/// By default, simply use the Boost.Serialization >> operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::load_fn_type get_type_handler_load(type<T>,
    overload_priority_weakest)
{
  return [](madara_iarchive &archive, void *ptr) {
      T &val = *static_cast<T *>(ptr);
      archive >> val;
    };
}

/// Creates a function for serializing the given type to a json_oarchive.
/// By default, simply use the Boost.Serialization << operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::save_json_fn_type
  get_type_handler_save_json(type<T>, overload_priority_weakest)
{
  return [](json_oarchive &archive, const void *ptr) {
      const T &val = *static_cast<const T *>(ptr);
      archive << val;
    };
}

/// Creates a function for unserializing the given type to a json_iarchive.
/// By default, simply use the Boost.Serialization >> operator.
/// Specialize this function to customize otherwise.
template<typename T>
constexpr TypeHandlers::load_json_fn_type
  get_type_handler_load_json(type<T>, overload_priority_weakest)
{
  return [](json_iarchive &archive, void *ptr) {
      T &val = *static_cast<T *>(ptr);
      archive >> val;
    };
}

/// Type trait for checking whether a type as an ADL-visible for_each_field
/// available.
MADARA_MAKE_VAL_SUPPORT_TEST(for_each_field, x,
    for_each_field(::madara::ignore_all<>{}, x));

/// Functor to pass to for_each_field to serialize a type
template<typename Archive>
struct do_serialize
{
  Archive *ar;

  template<typename T>
  void operator()(const char *name, T &val)
  {
    (*ar)(cereal::make_nvp(name, val));
  }
};

} // namespace knowledge

namespace exceptions {
  /**
   * An exception for misuse of the Any class
   **/
  class BadAnyAccess : public MadaraException
  {
  public:
    using MadaraException::MadaraException;

    template<typename Got>
    BadAnyAccess(type<Got>, knowledge::type_index expected)
      : BadAnyAccess(std::string("Bad Any access: expected ") +
#ifdef MADARA_USE_BOOST_TYPE_INDEX
          expected.pretty_name() + ", got " +
          knowledge::type_id<Got>().pretty_name()
#else
          expected.name() + ", got " +
          knowledge::type_id<Got>().name()
#endif
          ) {}
  };
}

namespace knowledge {

/// For internal use. Constructs a TypeHandlers containing functions used by Any
template<typename T>
inline const TypeHandlers &get_type_handler(type<T> t)
{
  static const TypeHandlers handler = {
      type_id<T>(),
      get_type_handler_destruct(t, select_overload()),
      get_type_handler_clone(t, select_overload()),
      get_type_handler_save(type<T>{}, select_overload()),
      get_type_handler_load(type<T>{}, select_overload()),
      get_type_handler_save_json(t, select_overload()),
      get_type_handler_load_json(t, select_overload()),
    };
  return handler;
}

/// For internal use. Constructs a TypeHandlers containing functions used by Any
template<typename T>
inline const TypeHandlers &get_type_handler()
{
  return get_type_handler(type<T>{});
}

/**
 * Type tag used with Any to signifiy storing raw unserialized data. The Any
 * will unserialize lazily as needed.
 *
 * Note that this lazy deserialization is not fully type-safe, and might not
 * throw an exception if the wrong type is used. The result may be garbled
 * data, but shouldn't segfault or trample other data.
 **/
constexpr struct raw_data_t {} raw_data;

/**
 * A general purpose type which can store any type which is:
 *
 *  * Default constructible
 *  * Copy constructible
 *  * Serializable by Boost.Serialization, or implements the for_each_field
 *    free function.
 *
 * This class is used by KnowledgeRecord and KnowledgeBase to store (nearly)
 * arbitrary types.
 *
 * It is similar in principle to Boost.Any (and C++17 std::any), but
 * incorporates serialization support, and a different interface.
 **/
class Any
{
public:
  /**
   * Default constructor. Creates an empty Any.
   **/
  Any() = default;

  /**
   * Copy constructor. Will clone any data stored inside.
   **/
  Any(const Any &other)
    : handler_(other.handler_),
      data_(other.data_ ?
        (handler_ ?
          handler_->clone(other.data_) :
          raw_data_storage::clone(other.data_)) : nullptr) {}

  /**
   * Copy assignment operator. Will clone any data stored inside.
   **/
  Any &operator=(const Any &other)
  {
    void *data = other.data_ ?
      (other.handler_ ?
        other.handler_->clone(other.data_) :
        raw_data_storage::clone(other.data_)) : nullptr;

    clear();
    handler_ = other.handler_;
    data_ = data;
    return *this;
  }

  /**
   * Move constructor. Other Any will be left empty.
   **/
  Any(Any &&other) noexcept :
    handler_(take_ptr(other.handler_)),
    data_(take_ptr(other.data_)) {}

  /**
   * Move assignment operator. Other Any will be left empty.
   **/
  Any &operator=(Any &&other) noexcept
  {
    if (this != &other) {
      using std::swap;
      swap(data_, other.data_);
      swap(handler_, other.handler_);
    }
    return *this;
  }

  /**
   * Destructor. Deletes the stored data.
   **/
  ~Any() noexcept
  {
    clear();
  }

  /**
   * Clear and free the data inside this Any. Safe to call if already empty.
   **/
  void clear() noexcept
  {
    if (!data_) {
      return;
    }
    if (handler_)
    {
      handler_->destruct((void*)data_);
      handler_ = nullptr;
    } else {
      delete [] (char*)data_;
    }
    data_ = nullptr;
  }

  /**
   * Construct from any compatible type. The argument will be moved into
   * the new Any if it supports it, and the argument is an rvalue reference.
   * Otherwise, it will be copied.
   **/
  template<typename T>
  explicit Any(T &&t, enable_if_<
    !is_type_tag<T>() &&
    !is_same_decayed<T, Any>(), int> = 0)
    : handler_(&get_type_handler(type<decay_<T>>{})),
      data_(reinterpret_cast<void*>(
            new decay_<T>(std::forward<T>(t))))
  {}

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The remaining arguments are forwarded to
   * that type's constructor arguments.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * // constructs a vector in-place with 42 copies of a string
   * Any a(type<std::vector<std::string>>{}, 42, "Don't panic");
   * @endcode
   **/
  template<typename T, typename... Args>
  explicit Any(type<T> t, Args&&... args)
    : handler_(&get_type_handler(t)),
      data_(reinterpret_cast<void*>(new T(std::forward<Args>(args)...))) {}

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The second argument is an initializer list
   * that will be passed along to that type's constructor.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * // constructs a vector in-place with several strings
   * Any a(type<std::vector<std::string>>{}, {"mostly", "harmless"});
   * @endcode
   **/
  template<typename T, typename I>
  explicit Any(type<T> t, std::initializer_list<I> init)
    : Any(t, init.begin(), init.end()) {}

  /**
   * Construct with serialized data, for lazy deserialization when first needed.
   *
   * Note that this lazy deserialization is not fully type-safe, and might not
   * throw an exception if the wrong type is used. The result may be garbled
   * data, but shouldn't segfault or trample other data.
   *
   * The first parameter is a type tag, which is available by-value from the
   * global `madara::knowledge::raw_data`.
   *
   * @param data a pointer to the serialized data to copy into this Any
   * @param size the amount of data to copy
   **/
  explicit Any(raw_data_t, const char *data, size_t size)
    : data_(raw_data_storage::make(data, size)) {}

  /**
   * Test whether this Any holds a value. A default constructed Any is empty.
   *
   * @return true if this Any is empty
   **/
  bool empty() const
  {
    return data_ == nullptr;
  }

  /**
   * Test whether this Any holds unserialized raw data, to be lazily
   * deserialized when first needed.
   *
   * Note that this lazy deserialization is not fully type-safe, and might not
   * throw an exception if the wrong type is used. The result may be garbled
   * data, but shouldn't segfault or trample other data.
   *
   * @return true if this Any holds raw data
   **/
  bool raw() const
  {
    return data_ != nullptr && handler_ == nullptr;
  }

  /**
   * Access the Any's stored value by reference.
   * If empty() is true, throw BadAnyAccess exception; else,
   * If raw() is true, try to deserialize using T, and store deserialized
   * data if successful, else throw BadAnyAccess exception.
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return *data_ as T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  T &ref(type<T> t)
  {
    if (!data_) {
      throw exceptions::BadAnyAccess("ref() called on empty Any");
    } else if (!handler_) {
      raw_data_storage *sto = (raw_data_storage *)data_;
      unserialize(t, sto->data, sto->size);
    } else if (type_id<T>() != handler_->tindex) {
      throw exceptions::BadAnyAccess(t, handler_->tindex);
    }
    return ref_unsafe(t);
  }

  /**
   * Access the Any's stored value by reference.
   * If empty() is true, throw BadAnyAccess exception; else,
   * If raw() is true, try to deserialize using T, and store deserialized
   * data if successful, else throw BadAnyAccess exception.
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  T &ref()
  {
    return ref(type<T>{});
  }

  /**
   * Access the Any's stored value by const reference.
   * If empty() or raw() is true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  const T &ref(type<T> t) const
  {
    if (!data_) {
      throw exceptions::BadAnyAccess("ref() called on empty Any");
    } else if (!handler_) {
      throw exceptions::BadAnyAccess("ref() called on const Any with raw data");
    } else if (type_id<T>() != handler_->tindex) {
      throw exceptions::BadAnyAccess(t, handler_->tindex);
    }
    return ref_unsafe(t);
  }

  /**
   * Access the Any's stored value by const reference.
   * If empty() or raw() is true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  const T &ref() const
  {
    return ref(type<T>{});
  }

  /**
   * Access the Any's stored value by const reference.
   * If empty() or raw() is true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  const T &cref(type<T> t) const
  {
    return ref(t);
  }

  /**
   * Access the Any's stored value by const reference.
   * If empty() or raw() is true, throw BadAnyAccess exception; else,
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * return the stored data as const T&, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return a reference to the contained value
   **/
  template<typename T>
  const T &cref() const
  {
    return ref(type<T>{});
  }

  /**
   * Take the Any's stored value, leaving it empty. On moveable types, this
   * will not copy the value.
   *
   * If empty() is true, throw BadAnyAccess exception; else,
   * If raw() is true, try to deserialize using T, and store deserialized
   * data if successful, else throw BadAnyAccess exception.
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * take and return the data, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return the formerly contained value
   **/
  template<typename T>
  T take(type<T> t)
  {
    T ret(std::move(ref(t)));
    clear();
    return ret;
  }

  /**
   * Take the Any's stored value, leaving it empty. On moveable types, this
   * will not copy the value.
   *
   * If empty() is true, throw BadAnyAccess exception; else,
   * If raw() is true, try to deserialize using T, and store deserialized
   * data if successful, else throw BadAnyAccess exception.
   * Otherwise, check type_id<T> matches handler_->tindex; if so,
   * take and return the data, else throw BadAnyAccess exception
   *
   * Note that T must match the type of the stored value exactly. It cannot
   * be a parent or convertible type, including primitive types.
   *
   * @return the formerly contained value
   **/
  template<typename T>
  T take()
  {
    return take(type<T>{});
  }

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The remaining arguments are forwarded to
   * that type's constructor arguments.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * Any a;
   * // constructs a vector in-place with 42 copies of a string
   * a.emplace(type<std::vector<std::string>>{}, 42, "Don't panic");
   * @endcode
   **/
  template<typename T, typename... Args>
  T &emplace(type<T>, Args&&... args)
  {
    std::unique_ptr<T> ptr(new T(std::forward<Args>(args)...));
    const auto &handler = get_type_handler(type<decay_<T>>{});

    clear();

    handler_ = &handler;
    T *ret = ptr.release();
    data_ = reinterpret_cast<void*>(ret);
    return *ret;
  }

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The second argument is an initializer list
   * that will be passed along to that type's constructor.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * Any a;
   * // constructs a vector in-place with several strings
   * a.emplace(type<std::vector<std::string>>{}, {"mostly", "harmless"});
   * @endcode
   **/
  template<typename T, typename I>
  T &emplace(type<T> t, std::initializer_list<I> init)
  {
    return emplace(t, init.begin(), init.end());
  }

  /**
   * Construct any compatible type in-place. The arguments are forwarded to
   * the given type's constructor arguments.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * Any a;
   * // constructs a vector in-place with 42 copies of a string
   * a.emplace<std::vector<std::string>>(42, "Don't panic");
   * @endcode
   **/
  template<typename T, typename... Args>
  T &emplace(Args&&... args)
  {
    return emplace(type<T>{}, std::forward<Args>(args)...);
  }

  /**
   * Construct any compatible type in-place. The first argument is a
   * madara::type struct which provides the type as a template parameter,
   * but otherwise holds no data. The second argument is an initializer list
   * that will be passed along to that type's constructor.
   *
   * Example:
   * @code{.cpp}
   * using madara::knowledge;
   *
   * Any a;
   * // constructs a vector in-place with several strings
   * a.emplace<std::vector<std::string>>({"mostly", "harmless"});
   * @endcode
   **/
  template<typename T, typename I>
  T &emplace(std::initializer_list<I> init)
  {
    return emplace(type<T>{}, init);
  }

  /**
   * Set from any compatible type. The argument will be moved into
   * this Any if it supports it, and the argument is an rvalue reference.
   * Otherwise, it will be copied.
   **/
  template<typename T>
  T &set(T &&t)
  {
    return emplace(type<decay_<T>>{},
           std::forward<T>(t));
  }

  /**
   * Store serialized data, for lazy deserialization when first needed.
   *
   * Note that this lazy deserialization is not fully type-safe, and might not
   * throw an exception if the wrong type is used. The result may be garbled
   * data, but shouldn't segfault or trample other data.
   *
   * @param data a pointer to the serialized data to copy into this Any
   * @param size the amount of data to copy
   **/
  void set_raw(const char *data, size_t size)
  {
    clear();
    data_ = raw_data_storage::make(data, size);
  }

  /**
   * Store serialized data, for lazy deserialization when first needed.
   *
   * Note that this lazy deserialization is not fully type-safe, and might not
   * throw an exception if the wrong type is used. The result may be garbled
   * data, but shouldn't segfault or trample other data.
   *
   * The first parameter is a type tag, which is available by-value from the
   * global `madara::knowledge::raw_data`.
   *
   * @param data a pointer to the serialized data to copy into this Any
   * @param size the amount of data to copy
   **/
  void set(raw_data_t, const char *data, size_t size)
  {
    set_raw(data, size);
  }

  /**
   * Reference the stored data as type T, WITHOUT any type checking. Do not
   * call this unless you are certain what type the Any contains. This does not
   * unserialize if the Any contains unserialized raw data.
   *
   * @tparam T the type to treat the stored data as
   * @return a reference of type T to the stored data
   **/
  template<typename T>
  T &ref_unsafe()
  {
    return ref_unsafe(type<T>{});
  }

  /**
   * Reference the stored data as type T, WITHOUT any type checking. Do not
   * call this unless you are certain what type the Any contains. This does not
   * unserialize if the Any contains unserialized raw data.
   *
   * @tparam T the type to treat the stored data as
   * @return a reference of type T to the stored data
   **/
  template<typename T>
  T &ref_unsafe(type<T>)
  {
    return *reinterpret_cast<T *>(data_);
  }

  /**
   * Reference the stored data as type T, WITHOUT any type checking. Do not
   * call this unless you are certain what type the Any contains. This does not
   * unserialize if the Any contains unserialized raw data.
   *
   * @tparam T the type to treat the stored data as
   * @return a const reference of type T to the stored data
   **/
  template<typename T>
  const T &ref_unsafe(type<T>) const
  {
    return *reinterpret_cast<const T *>(data_);
  }

  /**
   * Serialize this Any to the given buffer. Throws an exception if the buffer
   * size is insufficient.
   *
   * @param data the buffer to serialize to
   * @param size size of the buffer
   * @return the actual number of bytes used during serialization
   **/
  size_t serialize(char *data, size_t size) const
  {
    namespace bio = boost::iostreams;

    bio::array_sink output_sink(data, size);
    bio::stream<bio::array_sink> output_stream(output_sink);

    auto pos = output_stream.tellp();
    serialize(output_stream);
    auto len = output_stream.tellp() - pos;

    return len;
  }

  /**
   * Serialize this Any to the given vector. The vector will be cleared first,
   * and resized as needed.
   *
   * @param vec the vector to serialize to, which will be cleared first
   * @return the actual number of bytes used during serialization
   **/
  size_t serialize(std::vector<char> &vec) const
  {
    namespace bio = boost::iostreams;

    vec.clear();
    auto output_sink = bio::back_inserter(vec);
    bio::stream<decltype(output_sink)> output_stream(output_sink);

    serialize(output_stream);

    return vec.size();
  }

  /**
   * Serialize this Any to the given output stream.
   *
   * @param stream the output stream to serialize to
   **/
  void serialize(std::ostream &stream) const
  {
    madara_oarchive archive(stream);

    serialize(archive);
  }

  /**
   * Serialize this Any to the given madara_oarchive.
   *
   * @param stream the output archive to serialize to
   **/
  void serialize(madara_oarchive &archive) const
  {
    if (handler_) {
      handler_->save(archive, data_);
    } else {
      raw_data_storage *sto = (raw_data_storage *)data_;
      archive.saveBinary<1>(sto->data, sto->size);
    }
  }

  /**
   * Serialize this Any to the given archive. Use this overload for
   * Boost.Serialization archives other than madara_oarchive.
   *
   * @param stream the output archive to serialize to
   **/
  void serialize(json_oarchive &archive) const
  {
    if (handler_) {
      handler_->save_json(archive, data_);
    } else {
      raw_data_storage *sto = (raw_data_storage *)data_;
      archive.saveBinaryValue(sto->data, sto->size);
    }
  }

  std::string to_json() const
  {
    std::ostringstream stream;
    {
      cereal::JSONOutputArchive json(stream);
      json.makeArray();
      serialize(json);
    }
    return stream.str();
  }

  /**
   * Unserialize the given type from the given character array, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  size_t unserialize(type<T> t, const char *data, size_t size)
  {
    namespace bio = boost::iostreams;

    bio::array_source input_source(data, size);
    bio::stream<bio::array_source> input_stream(input_source);

    auto pos = input_stream.tellg();
    unserialize(t, input_stream);
    auto len = input_stream.tellg() - pos;

    return len;
  }

  /**
   * Unserialize the given type from the given character array, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  size_t unserialize(const char *data, size_t size)
  {
    return unserialize(type<T>{}, data, size);
  }

  /**
   * Unserialize the given type from the given input stream, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(type<T> t, std::istream &stream)
  {
    madara_iarchive archive(stream);

    unserialize(t, archive);
  }

  /**
   * Unserialize the given type from the given input stream, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(std::istream &stream)
  {
    unserialize(type<T>{}, stream);
  }

  /**
   * Unserialize the given type from the given madara_iarchive, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(type<T> t, madara_iarchive &archive)
  {
    const TypeHandlers &handler = get_type_handler(t);
    std::unique_ptr<T> ptr(new T{});

    handler.load(archive, (void*)ptr.get());

    clear();
    data_ = reinterpret_cast<void*>(ptr.release());
    handler_ = &handler;
  }

  /**
   * Unserialize the given type from the given madara_iarchive, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(madara_iarchive &archive)
  {
    unserialize(type<T>{}, archive);
  }

  /**
   * Unserialize the given type from the given archive, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(type<T> t, json_iarchive &archive)
  {
    const TypeHandlers &handler = get_type_handler(t);
    std::unique_ptr<T> ptr(new T{});

    handler.load_json(archive, (void*)ptr.get());

    clear();
    data_ = reinterpret_cast<void*>(ptr.release());
    handler_ = &handler;
  }

  /**
   * Unserialize the given type from the given archive, and store into
   * this Any. This operation provides the strong exception-guarantee: if an
   * exception is throw during unserialization, this Any will not be modified.
   **/
  template<typename T>
  void unserialize(json_iarchive &archive)
  {
    unserialize(type<T>{}, archive);
  }

private:
  const TypeHandlers *handler_ = nullptr;
  void *data_ = nullptr;

  struct raw_data_storage
  {
    size_t size;
    char data[1];

    static void *make(const char *data, size_t size)
    {
      auto ret = new char[size + sizeof(size)];
      raw_data_storage *sto = (raw_data_storage *)ret;
      sto->size = size;
      memcpy(&sto->data, data, size);
      return ret;
    }

    static void *clone(const void *orig)
    {
      raw_data_storage *sto = (raw_data_storage *)orig;
      return make(sto->data, sto->size);
    }
  };

  template<typename T>
  static T *take_ptr(T *&in)
  {
    T *ret = in;
    in = nullptr;
    return ret;
  }

  template<typename T>
  static const T *take_ptr(const T *&in)
  {
    const T *ret = in;
    in = nullptr;
    return ret;
  }
};

} } // namespace madara::knowledge

namespace cereal
{

// Implement Cereal library serialization for types supporting for_each_field
template<typename Archive, typename T>
auto serialize(Archive &ar, T &&val) ->
  ::madara::enable_if_<::madara::knowledge::supports_for_each_field<T>::value>
{
  for_each_field(::madara::knowledge::do_serialize<Archive>{&ar},
      std::forward<T>(val));
}

}

#endif  // MADARA_KNOWLEDGE_ANY_H_