#pragma once
#include "ns3/scion-address.h"
#include "ns3/scion-snet-path.h"
#include "ns3/scion-utils.h"

#include <vector>
#include <map>
#include <memory>

namespace ns3
{

class SCIONReplySelector
{ // ReplySelector controls the reply path in a "listening" socket.

    class SCIONReplySelectorConcept
    {
      public:
        // selects the Path for the next packet to remote.
        // Invoked for each packet sent with WriteTo()
        virtual const SNETPath& Path(const SCIONAddress& remote) const = 0;

        // Initialize the selector
        // invoked once during the creation of a ListenConn
        virtual void Initialize(const SCIONAddress& local) = 0;

        // record a path used by the remote for a packet received.
        // Invoked whenever a packet is received
        // THe path is reversed i.e. its the path from here to remote
        virtual void Record(const SNETPath& path, const SCIONAddress& remote) = 0;

        // PathDown is called whenever an SCMP DownNotification is received on any
        // connection so that the selector can adapt its path choice.
        // The downNotification may be for unrelated paths not used by this selector
        virtual void PathDown(PathHash_t hash, PathInterface pi) = 0;
        virtual void Close() = 0;
    };

    template <class T>
    struct ReplySelectorModel : public SCIONReplySelectorConcept
    {
        //   PathModel(  T&& t ) : object( std::forward<T>(t) ) {}

        explicit ReplySelectorModel(T& value)
            : object(_NonConstReference(value))
        {
        }

        // explicit PathSelectorModel( T const& value) : object(_ConstReference(value)){} // makes
        // little sense here ^^ remove later
        explicit ReplySelectorModel(T&& value)
            : object(_Value(std::move(value)))
        {
        }

        virtual ~ReplySelectorModel()
        {
        }

        virtual const SNETPath& Path(const SCIONAddress& remote) const
        {
          return  static_cast<const T&>(object).Path(remote);
        }

        virtual void Initialize(const SCIONAddress& local)
        {
            static_cast<T&>(object).Initialize(local);
        }

        virtual void Record(const SNETPath& path, const SCIONAddress& remote)
        {
            static_cast<T&>(object).Refresh(path, remote);
        }

        virtual void PathDown(PathHash_t hash, PathInterface pi)
        {
            static_cast<T&>(object).PathDown(hash, pi);
        }

        virtual void Close()
        {
            static_cast<T&>(object).Close();
        }

        Storage<T> object;
    };

    std::shared_ptr<SCIONReplySelectorConcept> object = nullptr;

  public:
    SCIONReplySelector() = default; // Path has to be default constructible,

    // in order for higher level objects such as SCIONHeader who contain a Path to be default
    // constructible

    template <typename T>
    SCIONReplySelector(T& obj)
        : object(std::make_shared<ReplySelectorModel<T>>(obj))
    {
    }

    template <typename T>
    SCIONReplySelector(T&& obj)
        : object(std::make_shared<ReplySelectorModel<T>>(std::move(obj)))
    {
    }

    const SNETPath& Path(const SCIONAddress& remote) const
    {
        if (object) // null if default constructed
            return object->Path(remote);
    }

    void Initialize(const SCIONAddress& local)
    {
        if (object)
        {
            object->Initialize(local);
        }
    }

    void Record(const SNETPath& path, const SCIONAddress& remote)
    {
        if (object)
        {
            object->Record(path, remote);
        }
    }

    void PathDown(PathHash_t hash, PathInterface pi)
    {
        if (object)
        {
            object->PathDown(hash, pi);
        }
    }

    void Close()
    {
        if (object)
        {
            object->Close();
        }
    }
};

class SCIONDefaultReplySelector
{
  public:
    // selects the Path for the next packet to remote.
    // Invoked for each packet sent with WriteTo()
    const SNETPath& Path(const SCIONAddress& remote) const;

    // Initialize the selector
    // invoked once during the creation of a ListenConn
    void Initialize(const SCIONAddress& local);

    // record a path used by the remote for a packet received.
    // Invoked whenever a packet is received
    // THe path is reversed i.e. its the path from here to remote
    void Record(const SNETPath& path, const SCIONAddress& remote);

    // PathDown is called whenever an SCMP DownNotification is received on any
    // connection so that the selector can adapt its path choice.
    // The downNotification may be for unrelated paths not used by this selector
    void PathDown(PathHash_t hash, PathInterface pi);
    void Close();

  private:
    struct remoteEntry_t
    { // list tracking the most recently used ( inserted ) path
        std::vector<SNETPath> m_paths;
        Time m_lastSeen;

        void insert(const SNETPath& path, size_t maxEntries);
    };

    std::map<SCIONAddress, remoteEntry_t> m_remotes;
    inline static const size_t DefaultReplySelectorMaxPaths = 4;
};

} // namespace ns3