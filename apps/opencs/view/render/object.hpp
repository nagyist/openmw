#ifndef OPENCS_VIEW_OBJECT_H
#define OPENCS_VIEW_OBJECT_H

#include <memory>
#include <string>

#include <osg/Vec3f>
#include <osg/Vec4f>
#include <osg/ref_ptr>

#include <components/esm/position.hpp>
#include <components/esm/refid.hpp>

#include "tagbase.hpp"

class QModelIndex;

namespace osg
{
    class PositionAttitudeTransform;
    class Geometry;
    class Group;
    class Node;
}

namespace osgFX
{
    class Scribe;
}

namespace Resource
{
    class ResourceSystem;
}

namespace CSMWorld
{
    class Data;
    struct CellRef;
    class CommandMacro;
}

namespace CSVRender
{
    class Actor;
    class Object;
    struct WorldspaceHitResult;

    // An object to attach as user data to the osg::Node, allows us to get an Object back from a Node when we are doing
    // a ray query
    class ObjectTag : public TagBase
    {
    public:
        ObjectTag(Object* object);

        Object* mObject;

        QString getToolTip(bool hideBasics, const WorldspaceHitResult& hit) const override;
    };

    class Object
    {
    public:
        enum OverrideFlags
        {
            Override_Position = 1,
            Override_Rotation = 2,
            Override_Scale = 4
        };

        enum SubMode
        {
            Mode_Move,
            Mode_Rotate,
            Mode_Scale,
            Mode_None,
        };

        enum Axis
        {
            Axis_X,
            Axis_Y,
            Axis_Z
        };

    private:
        CSMWorld::Data& mData;
        ESM::RefId mReferenceId;
        ESM::RefId mReferenceableId;
        osg::ref_ptr<osg::PositionAttitudeTransform> mRootNode;
        osg::ref_ptr<osg::PositionAttitudeTransform> mBaseNode;
        osg::ref_ptr<osgFX::Scribe> mOutline;
        bool mSelected;
        bool mSnapTarget;
        osg::Group* mParentNode;
        Resource::ResourceSystem* mResourceSystem;
        bool mForceBaseToZero;
        ESM::Position mPositionOverride;
        float mScaleOverride;
        int mOverrideFlags;
        std::unique_ptr<Actor> mActor;

        /// Not implemented
        Object(const Object&);

        /// Not implemented
        Object& operator=(const Object&);

        /// Remove object from node (includes deleting)
        void clear();

        /// Update model
        /// @note Make sure adjustTransform() was called first so world space particles get positioned correctly
        void update();

        /// Adjust position, orientation and scale
        void adjustTransform();

        /// Throws an exception if *this was constructed with referenceable
        const CSMWorld::CellRef& getReference() const;

    public:
        Object(CSMWorld::Data& data, osg::Group* cellNode, const std::string& id, bool referenceable,
            bool forceBaseToZero = false);
        /// \param forceBaseToZero If this is a reference ignore the coordinates and place
        /// it at 0, 0, 0 instead.

        ~Object();

        /// Mark the object as selected, selected objects show an outline effect
        void setSelected(bool selected, const osg::Vec4f& color = osg::Vec4f(1, 1, 1, 1));

        bool getSelected() const;

        /// Mark Object as "snap target"
        void setSnapTarget(bool isSnapTarget);

        bool getSnapTarget() const;

        /// Get object node with GUI graphics
        osg::ref_ptr<osg::Group> getRootNode();

        /// Get object node without GUI graphics
        osg::ref_ptr<osg::Group> getBaseNode();

        /// \return Did this call result in a modification of the visual representation of
        /// this object?
        bool referenceableDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

        /// \return Did this call result in a modification of the visual representation of
        /// this object?
        bool referenceableAboutToBeRemoved(const QModelIndex& parent, int start, int end);

        /// \return Did this call result in a modification of the visual representation of
        /// this object?
        bool referenceDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

        /// Reloads the underlying asset
        void reloadAssets();

        /// Returns an empty string if this is a refereceable-type object.
        std::string getReferenceId() const;

        std::string getReferenceableId() const;

        osg::ref_ptr<TagBase> getTag() const;

        /// Is there currently an editing operation running on this object?
        bool isEdited() const;

        void setEdited(int flags);

        ESM::Position getPosition() const;

        float getScale() const;

        /// Set override position.
        void setPosition(const float position[3]);

        /// Set override rotation
        void setRotation(const float rotation[3]);

        /// Set override scale
        void setScale(float scale);

        void setMarkerTransparency(float value);

        /// Apply override changes via command and end edit mode
        void apply(CSMWorld::CommandMacro& commands);

        /// Erase all overrides and restore the visual representation of the object to its
        /// true state.
        void reset();
    };
}

#endif
