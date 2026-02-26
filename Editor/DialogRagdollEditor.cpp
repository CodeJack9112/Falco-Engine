#include "stdafx.h"
#include "DialogRagdollEditor.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

#include "../Engine/RigidBody.h"
#include "../Engine/BoxCollider.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/CharacterJoint.h"
#include "../Engine/Mathf.h"

#include "MainWindow.h"
#include "ConsoleWindow.h"
#include "TreeNode.h"

DialogRagdollEditor::DialogRagdollEditor()
{
}

DialogRagdollEditor::~DialogRagdollEditor()
{
}

void DialogRagdollEditor::show()
{
	visible = true;
}

void DialogRagdollEditor::update()
{
	if (!visible)
		return;

	ImGui::SetNextWindowSize(ImVec2(300.0f, 430.0f));
	ImGui::SetNextWindowPos(ImVec2(MainWindow::GetWindowSize().x / 2, MainWindow::GetWindowSize().y / 2), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
	ImGui::Begin("Create Ragdoll", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking);

	string pelvisText = "None";
	string lhipText = "None";
	string lkneeText = "None";
	string rhipText = "None";
	string rkneeText = "None";
	string larmText = "None";
	string lelbowText = "None";
	string rarmText = "None";
	string relbowText = "None";
	string mspineText = "None";
	string headText = "None";

	if (pelvisObject != nullptr) pelvisText = pelvisObject->getAlias();
	if (lhipObject != nullptr) lhipText = lhipObject->getAlias();
	if (lkneeObject != nullptr) lkneeText = lkneeObject->getAlias();
	if (rhipObject != nullptr) rhipText = rhipObject->getAlias();
	if (rkneeObject != nullptr) rkneeText = rkneeObject->getAlias();
	if (larmObject != nullptr) larmText = larmObject->getAlias();
	if (lelbowObject != nullptr) lelbowText = lelbowObject->getAlias();
	if (rarmObject != nullptr) rarmText = rarmObject->getAlias();
	if (relbowObject != nullptr) relbowText = relbowObject->getAlias();
	if (mspineObject != nullptr) mspineText = mspineObject->getAlias();
	if (headObject != nullptr) headText = headObject->getAlias();

	ImGui::Text("Pelvis"); ImGui::SameLine();
	ImGui::InputText("##1", &pelvisText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(pelvisObject);

	ImGui::Text("Left Hip"); ImGui::SameLine();
	ImGui::InputText("##2", &lhipText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(lhipObject);

	ImGui::Text("Left Knee"); ImGui::SameLine();
	ImGui::InputText("##3", &lkneeText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(lkneeObject);

	ImGui::Text("Right Hip"); ImGui::SameLine();
	ImGui::InputText("##4", &rhipText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(rhipObject);

	ImGui::Text("Right Knee"); ImGui::SameLine();
	ImGui::InputText("##5", &rkneeText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(rkneeObject);

	ImGui::Text("Left Arm"); ImGui::SameLine();
	ImGui::InputText("##6", &larmText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(larmObject);

	ImGui::Text("Left Elbow"); ImGui::SameLine();
	ImGui::InputText("##7", &lelbowText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(lelbowObject);

	ImGui::Text("Right Arm"); ImGui::SameLine();
	ImGui::InputText("##8", &rarmText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(rarmObject);

	ImGui::Text("Right Elbow"); ImGui::SameLine();
	ImGui::InputText("##9", &relbowText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(relbowObject);

	ImGui::Text("Middle Spine"); ImGui::SameLine();
	ImGui::InputText("##10", &mspineText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(mspineObject);

	ImGui::Text("Head"); ImGui::SameLine();
	ImGui::InputText("##11", &headText, ImGuiInputTextFlags_ReadOnly);
	acceptDragDrop(headObject);

	ImGui::Text("Total Mass"); ImGui::SameLine();
	ImGui::InputFloat("##12", &massValue);

    string errorString = CheckConsistency();
    CalculateAxes();
    string helpString = "";

    if (errorString.length() != 0)
    {
        helpString = "Drag all bones from the hierarchy into their slots.\nMake sure your character is in T-Stand.\n";
    }
    else
    {
        helpString = "Make sure your character is in T-Stand.\nMake sure the blue axis faces in the same\ndirection the character is looking.";
    }

    ImGui::TextColored(ImVec4(1.0f / 255.0f * 158.0f, 1.0f / 255.0f * 195.0f, 1.0f, 1.0f), helpString.c_str());

    if (!errorString.empty())
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorString.c_str());

    bool isValid = errorString.length() == 0;

    bool closed = false;
    bool ok = false;

    ImGui::Separator();

    if (ImGui::Button("Clear"))
    {
        pelvisObject = nullptr;
        lhipObject = nullptr;
        lkneeObject = nullptr;
        rhipObject = nullptr;
        rkneeObject = nullptr;
        larmObject = nullptr;
        lelbowObject = nullptr;
        rarmObject = nullptr;
        relbowObject = nullptr;
        mspineObject = nullptr;
        headObject = nullptr;
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel"))
    {
        closed = true;
        ok = false;
    }

    if (isValid)
    {
        ImGui::SameLine();

        if (ImGui::Button("Create"))
        {
            closed = true;
            ok = true;
        }
    }

    ImGui::End();

	if (closed && ok)
	{
		createRagdoll();
	}

	if (closed)
		visible = false;
}

void DialogRagdollEditor::acceptDragDrop(SceneNode*& sceneNode)
{
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_TreeView"))
		{
			TreeNode* move_from = (TreeNode*)payload->Data;
			sceneNode = GetEngine->GetSceneManager()->getSceneNode(move_from->name);
		}
		ImGui::EndDragDropTarget();
	}
}

void DialogRagdollEditor::createRagdoll()
{
    Cleanup(pelvisObject);

    BuildCapsules();
    AddBreastColliders();
    AddHeadCollider();

    BuildBodies();
    BuildJoints();
    CalculateMass();

    MainWindow::gizmo2->showWireframe(true);
}

string DialogRagdollEditor::CheckConsistency()
{
    PrepareBones();
    std::map<SceneNode*, BoneInfo*> map;

    for(BoneInfo * bone : bones)
    {
        if (bone->anchor)
        {
            if (map[bone->anchor] != nullptr)
            {
                BoneInfo * oldBone = (BoneInfo*)map[bone->anchor];
                return bone->name + " and " + oldBone->name + "\nmay not be assigned to the same bone.";
            }
            map[bone->anchor] = bone;
        }
    }

    for(BoneInfo * bone : bones)
    {
        if (bone->anchor == nullptr)
            return bone->name + " has not been assigned yet.\n";
    }

    return "";
}

void DialogRagdollEditor::DecomposeVector(Vector3 & normalCompo, Vector3 & tangentCompo, Vector3 outwardDir, Vector3 outwardNormal)
{
    outwardNormal = outwardNormal.normalisedCopy();
    normalCompo = outwardNormal * outwardDir.dotProduct(outwardNormal);
    tangentCompo = outwardDir - normalCompo;
}

void DialogRagdollEditor::CalculateAxes()
{
    if (headObject != nullptr && pelvisObject != nullptr)
        up = CalculateDirectionAxis(Mathf::inverseTransformPoint(pelvisObject, headObject->_getDerivedPosition()));

    if (relbowObject != nullptr && pelvisObject != nullptr)
    {
        Vector3 removed, temp;
        DecomposeVector(temp, removed, Mathf::inverseTransformPoint(pelvisObject, relbowObject->_getDerivedPosition()), up);
        right = CalculateDirectionAxis(removed);
    }

    forward = right.crossProduct(up);
    if (flipForward)
        forward = -forward;
}

void DialogRagdollEditor::PrepareBones()
{
    if (pelvisObject)
    {
        /*worldRight = pelvis.TransformDirection(right);
        worldUp = pelvis.TransformDirection(up);
        worldForward = pelvis.TransformDirection(forward);*/

        worldRight = pelvisObject->_getDerivedOrientation() * Vector3(1, 0, 0);
        worldUp = pelvisObject->_getDerivedOrientation() * Vector3(0, 1, 0);
        worldForward = pelvisObject->_getDerivedOrientation() * Vector3(0, 0, 1);
    }

    bones = std::vector<BoneInfo*>();

    rootBone = new BoneInfo();
    rootBone->name = "Pelvis";
    rootBone->anchor = pelvisObject;
    rootBone->parent = nullptr;
    rootBone->density = 2.5F;
    bones.push_back(rootBone);

    Vector3 minLimit = Vector3(Degree(-10).valueRadians(), Degree(-50).valueRadians(), Degree(-180).valueRadians());
    Vector3 maxLimit = Vector3(Degree(20).valueRadians(), Degree(30).valueRadians(), Degree(180).valueRadians());

    AddMirroredJoint("Hips", lhipObject, rhipObject, "Pelvis", worldRight, worldForward, minLimit, maxLimit, 30, "CapsuleCollider", 0.19F, 1.5F);

    minLimit = Vector3(Degree(0).valueRadians(), Degree(0).valueRadians(), Degree(-100).valueRadians());
    maxLimit = Vector3(Degree(0).valueRadians(), Degree(0).valueRadians(), Degree(0).valueRadians());

    AddMirroredJoint("Knee", lkneeObject, rkneeObject, "Hips", worldRight, worldForward, minLimit, maxLimit, 0, "CapsuleCollider", 0.20F, 1.5F);

    minLimit = Vector3(Degree(-20).valueRadians(), Degree(-10).valueRadians(), Degree(-20).valueRadians());
    maxLimit = Vector3(Degree(20).valueRadians(), Degree(10).valueRadians(), Degree(20).valueRadians());

    AddJoint("Middle Spine", mspineObject, "Pelvis", worldRight, worldForward, minLimit, maxLimit, 10, "", 0.40f, 2.5F);

    minLimit = Vector3(Degree(-90).valueRadians(), Degree(-90).valueRadians(), Degree(-5).valueRadians());
    maxLimit = Vector3(Degree(90).valueRadians(), Degree(90).valueRadians(), Degree(5).valueRadians());

    AddMirroredJoint("Arm", larmObject, rarmObject, "Middle Spine", worldUp, worldForward, minLimit, maxLimit, 50, "CapsuleCollider", 0.25F, 1.0F);

    minLimit = Vector3(Degree(0).valueRadians(), Degree(-70).valueRadians(), Degree(-5).valueRadians());
    maxLimit = Vector3(Degree(0).valueRadians(), Degree(0).valueRadians(), Degree(5).valueRadians());

    //AddMirroredJoint("Elbow", lelbowObject, relbowObject, "Arm", worldForward, worldUp, minLimit, maxLimit, 0, "CapsuleCollider", 0.15F, 1.0F);
    AddJoint("Left Elbow", lelbowObject, "Arm", worldForward, worldUp, minLimit, maxLimit, 0, "CapsuleCollider", 0.15F, 1.0F);

    minLimit = Vector3(Degree(0).valueRadians(), Degree(0).valueRadians(), Degree(-5).valueRadians());
    maxLimit = Vector3(Degree(0).valueRadians(), Degree(70).valueRadians(), Degree(5).valueRadians());

    AddJoint("Right Elbow", relbowObject, "Arm", worldForward, worldUp, minLimit, maxLimit, 0, "CapsuleCollider", 0.15F, 1.0F);

    minLimit = Vector3(Degree(-20).valueRadians(), Degree(-80).valueRadians(), Degree(-10).valueRadians());
    maxLimit = Vector3(Degree(20).valueRadians(), Degree(80).valueRadians(), Degree(10).valueRadians());

    AddJoint("Head", headObject, "Middle Spine", worldRight, worldForward, minLimit, maxLimit, 25, "", 1, 1.0F);
}

BoneInfo * DialogRagdollEditor::FindBone(string name)
{
    for (BoneInfo * bone : bones)
    {
        if (bone->name == name)
            return bone;
    }
    return nullptr;
}

void DialogRagdollEditor::AddMirroredJoint(string name, SceneNode* leftAnchor, SceneNode* rightAnchor, string parent, Vector3 worldTwistAxis, Vector3 worldSwingAxis, Vector3 minLimit, Vector3 maxLimit, float swingLimit, string colliderType, float radiusScale, float density)
{
    AddJoint("Left " + name, leftAnchor, parent, worldTwistAxis, worldSwingAxis, minLimit, maxLimit, swingLimit, colliderType, radiusScale, density);
    AddJoint("Right " + name, rightAnchor, parent, worldTwistAxis, worldSwingAxis, minLimit, maxLimit, swingLimit, colliderType, radiusScale, density);
}

void DialogRagdollEditor::AddJoint(string name, SceneNode* anchor, string parent, Vector3 worldTwistAxis, Vector3 worldSwingAxis, Vector3 minLimit, Vector3 maxLimit, float swingLimit, string colliderType, float radiusScale, float density)
{
    BoneInfo * bone = new BoneInfo();
    bone->name = name;
    bone->anchor = anchor;
    bone->axis = worldTwistAxis;
    bone->normalAxis = worldSwingAxis;
    bone->minLimit = minLimit;
    bone->maxLimit = maxLimit;
    bone->swingLimit = swingLimit;
    bone->density = density;
    bone->colliderType = colliderType;
    bone->radiusScale = radiusScale;

    if (FindBone(parent) != nullptr)
        bone->parent = FindBone(parent);
    else if (name._Starts_with("Left"))
        bone->parent = FindBone("Left " + parent);
    else if (name._Starts_with("Right"))
        bone->parent = FindBone("Right " + parent);

    bone->parent->children.push_back(bone);
    bones.push_back(bone);
}

void GetChildren(SceneNode * root, std::vector<SceneNode*> & outList)
{
    VectorIterator it = root->getChildIterator();
    while (it.hasMoreElements())
    {
        SceneNode* child = (SceneNode*)it.getNext();
        outList.push_back(child);
        GetChildren(child, outList);
    }
}

void DialogRagdollEditor::BuildCapsules()
{
    for (BoneInfo * bone : bones)
    {
        if (bone->colliderType != "CapsuleCollider")
            continue;

        int direction = 0;
        float distance = 0;

        if (bone->children.size() == 1)
        {
            BoneInfo * childBone = bone->children[0];
            Vector3 endPoint = Mathf::inverseTransformPoint(bone->anchor, childBone->anchor->_getDerivedPosition());
            CalculateDirection(endPoint, direction, distance);
        }
        else
        {
            Vector3 endPoint = Mathf::inverseTransformPoint(bone->anchor, (bone->anchor->_getDerivedPosition() - bone->parent->anchor->_getDerivedPosition()) + bone->anchor->_getDerivedPosition());
            CalculateDirection(endPoint, direction, distance);

            Bounds bounds;
            std::vector<SceneNode*> transforms;
            GetChildren(bone->anchor, transforms);

            for (SceneNode * child : transforms)
            {
                bounds.Encapsulate(Mathf::inverseTransformPoint(bone->anchor, child->_getDerivedPosition()));
            }

            if (distance > 0)
                distance = bounds.getMax()[direction];
            else
                distance = bounds.getMin()[direction];

        }

        CapsuleCollider* collider = new CapsuleCollider(bone->anchor);
        Vector3 center = Vector3::ZERO;

        if (direction == 0)
            collider->SetRotation(Mathf::toQuaternion(Vector3(90, 0, 0)));

        if (direction == 1)
            collider->SetRotation(Mathf::toQuaternion(Vector3(0, 90, 0)));

        if (direction == 2)
            collider->SetRotation(Mathf::toQuaternion(Vector3(0, 0, 90)));

        center[direction] = distance * 0.5f;

        collider->SetOffset(center);
        collider->SetHeight(abs(distance * 0.42f));
        collider->SetRadius(abs(distance * bone->radiusScale));

        bone->direction = direction;

        bone->anchor->AddComponent(collider);
    }
}

void DialogRagdollEditor::Cleanup(SceneNode * root)
{
    CharacterJoint* joint = (CharacterJoint*)root->GetComponent(CharacterJoint::COMPONENT_TYPE);
    while (joint != nullptr)
    {
        root->RemoveComponent(joint);
        joint = (CharacterJoint*)root->GetComponent(CharacterJoint::COMPONENT_TYPE);
    }

    RigidBody* body = (RigidBody*)root->GetComponent(RigidBody::COMPONENT_TYPE);
    while (body != nullptr)
    {
        root->RemoveComponent(body);
        body = (RigidBody*)root->GetComponent(RigidBody::COMPONENT_TYPE);
    }

    CapsuleCollider* capsule = (CapsuleCollider*)root->GetComponent(CapsuleCollider::COMPONENT_TYPE);
    while (capsule != nullptr)
    {
        root->RemoveComponent(capsule);
        capsule = (CapsuleCollider*)root->GetComponent(CapsuleCollider::COMPONENT_TYPE);
    }

    SphereCollider* sphere = (SphereCollider*)root->GetComponent(SphereCollider::COMPONENT_TYPE);
    while (sphere != nullptr)
    {
        root->RemoveComponent(sphere);
        sphere = (SphereCollider*)root->GetComponent(SphereCollider::COMPONENT_TYPE);
    }

    BoxCollider* box = (BoxCollider*)root->GetComponent(BoxCollider::COMPONENT_TYPE);
    while (box != nullptr)
    {
        root->RemoveComponent(box);
        box = (BoxCollider*)root->GetComponent(BoxCollider::COMPONENT_TYPE);
    }

    VectorIterator it = root->getChildIterator();

    while (it.hasMoreElements())
    {
        SceneNode* node = (SceneNode*)it.getNext();

        Cleanup(node);
    }
}

void DialogRagdollEditor::BuildBodies()
{
    for (BoneInfo * bone : bones)
    {
        RigidBody* body = new RigidBody(bone->anchor);
        body->SetMass(bone->density);
        bone->anchor->AddComponent(body);
    }
}

void DialogRagdollEditor::BuildJoints()
{
    for (BoneInfo * bone : bones)
    {
        if (bone->parent == nullptr)
            continue;

        CharacterJoint* joint = new CharacterJoint(bone->anchor);
        bone->anchor->AddComponent(joint);

        bone->joint = joint;

        // Setup connection and axis
        //joint->SetAxis(CalculateDirectionAxis(Mathf::inverseTransformDirection(bone->anchor, bone->axis)));
        //joint.swingAxis = CalculateDirectionAxis(bone->normalAxis));
        joint->SetAnchor(Vector3::ZERO);
        
        if (bone->parent != nullptr)
        {
            Quaternion r = Quaternion::IDENTITY;

            Vector3 p = Mathf::inverseTransformPoint(bone->parent->anchor, bone->anchor->_getDerivedPosition()/*, bone->parent->anchor->_getDerivedScale()*/);
            joint->SetConnectedAnchor(p);
        }

        joint->SetConnectedObjectName(bone->parent->anchor->getName());
        joint->SetLimitMin(bone->minLimit);
        joint->SetLimitMax(bone->maxLimit);
        //joint->SetLinkedBodiesCollision(false);
    }
}

void DialogRagdollEditor::CalculateMassRecurse(BoneInfo * bone)
{
    float mass = ((RigidBody*)bone->anchor->GetComponent(RigidBody::COMPONENT_TYPE))->GetMass();
    for (BoneInfo * child : bone->children)
    {
        CalculateMassRecurse(child);
        mass += child->summedMass;
    }
    bone->summedMass = mass;
}

void DialogRagdollEditor::CalculateMass()
{
    // Calculate allChildMass by summing all bodies
    CalculateMassRecurse(rootBone);

    // Rescale the mass so that the whole character weights totalMass
    float massScale = massValue / rootBone->summedMass;
    for (BoneInfo* bone : bones)
    {
        RigidBody* b = ((RigidBody*)bone->anchor->GetComponent(RigidBody::COMPONENT_TYPE));
        b->SetMass(b->GetMass() * massScale);
    }

    // Recalculate allChildMass by summing all bodies
    CalculateMassRecurse(rootBone);
}

void DialogRagdollEditor::CalculateDirection(Vector3 point, int & direction, float & distance)
{
    // Calculate longest axis
    direction = 0;
    if (abs(point[1]) > abs(point[0]))
        direction = 1;
    if (abs(point[2]) > abs(point[direction]))
        direction = 2;

    distance = point[direction];
}

Vector3 DialogRagdollEditor::CalculateDirectionAxis(Vector3 point)
{
    int direction = 0;
    float distance;
    CalculateDirection(point, direction, distance);
    Vector3 axis = Vector3::ZERO;
    if (distance > 0)
        axis[direction] = 1.0F;
    else
        axis[direction] = -1.0F;
    return axis;
}

int DialogRagdollEditor::SmallestComponent(Vector3 point)
{
    int direction = 0;
    if (abs(point[1]) < abs(point[0]))
        direction = 1;
    if (abs(point[2]) < abs(point[direction]))
        direction = 2;
    return direction;
}

int DialogRagdollEditor::LargestComponent(Vector3 point)
{
    int direction = 0;
    if (abs(point[1]) > abs(point[0]))
        direction = 1;
    if (abs(point[2]) > abs(point[direction]))
        direction = 2;
    return direction;
}

int DialogRagdollEditor::SecondLargestComponent(Vector3 point)
{
    int smallest = SmallestComponent(point);
    int largest = LargestComponent(point);
    if (smallest < largest)
    {
        int temp = largest;
        largest = smallest;
        smallest = temp;
    }

    if (smallest == 0 && largest == 1)
        return 2;
    else if (smallest == 0 && largest == 2)
        return 1;
    else
        return 0;
}

Bounds DialogRagdollEditor::Clip(Bounds bounds, SceneNode * relativeTo, SceneNode* clipTransform, bool below)
{
    int axis = LargestComponent(bounds.getSize());
    Vector3 v1 = Mathf::transformPoint(relativeTo, bounds.getMax());
    Vector3 v2 = Mathf::transformPoint(relativeTo, bounds.getMin());

    if (worldUp.dotProduct(v1) > worldUp.dotProduct(v2) == below)
    {
        Vector3 min = bounds.getMin();
        min[axis] = Mathf::inverseTransformPoint(relativeTo, clipTransform->_getDerivedPosition())[axis];
        
        bounds.setMin(min);
    }
    else
    {
        Vector3 max = bounds.getMax();
        max[axis] = Mathf::inverseTransformPoint(relativeTo, clipTransform->_getDerivedPosition())[axis];
        
        bounds.setMax(max);
    }

    return bounds;
}

Bounds DialogRagdollEditor::GetBreastBounds(SceneNode* relativeTo)
{
    // Pelvis bounds
    Bounds bounds;
    
    bounds.Encapsulate(Mathf::inverseTransformPoint(relativeTo, lhipObject->_getDerivedPosition()));
    bounds.Encapsulate(Mathf::inverseTransformPoint(relativeTo, rhipObject->_getDerivedPosition()));
    bounds.Encapsulate(Mathf::inverseTransformPoint(relativeTo, larmObject->_getDerivedPosition()));
    bounds.Encapsulate(Mathf::inverseTransformPoint(relativeTo, rarmObject->_getDerivedPosition()));

    Vector3 size = bounds.getSize();
    size[SmallestComponent(bounds.getSize())] = size[LargestComponent(bounds.getSize())] / 2.0F;
    bounds.setSize(size);

    return bounds;
}

void DialogRagdollEditor::AddBreastColliders()
{
    // Middle spine and pelvis
    if (mspineObject != nullptr && pelvisObject != nullptr)
    {
        Bounds bounds;
        BoxCollider * box = nullptr;

        // Middle spine bounds
        bounds = Clip(GetBreastBounds(pelvisObject), pelvisObject, mspineObject, false);

        box = new BoxCollider(pelvisObject);
        box->SetOffset(bounds.getCenter());
        box->SetBoxSize(Vector3(abs(bounds.getSize().x), abs(bounds.getSize().y), abs(bounds.getSize().z)) * 0.40f);
        pelvisObject->AddComponent(box);

        bounds = Bounds();
        bounds = Clip(GetBreastBounds(mspineObject), mspineObject, mspineObject, true);

        box = new BoxCollider(mspineObject);
        box->SetOffset(bounds.getCenter());
        box->SetBoxSize(Vector3(abs(bounds.getSize().x), abs(bounds.getSize().y), abs(bounds.getSize().z)) * 0.40f);
        mspineObject->AddComponent(box);
    }
    // Only pelvis
    else
    {
        Bounds bounds;

        bounds.Encapsulate(Mathf::inverseTransformPoint(pelvisObject, lhipObject->_getDerivedPosition()));
        bounds.Encapsulate(Mathf::inverseTransformPoint(pelvisObject, rhipObject->_getDerivedPosition()));
        bounds.Encapsulate(Mathf::inverseTransformPoint(pelvisObject, larmObject->_getDerivedPosition()));
        bounds.Encapsulate(Mathf::inverseTransformPoint(pelvisObject, rarmObject->_getDerivedPosition()));

        Vector3 size = bounds.getSize();
        size[SmallestComponent(bounds.getSize())] = size[LargestComponent(bounds.getSize())] / 2.0F;

        BoxCollider * box = new BoxCollider(pelvisObject);
        pelvisObject->AddComponent(box);
        box->SetOffset(bounds.getCenter());
        box->SetBoxSize(Vector3(abs(size.x), abs(size.y), abs(size.z)) * 0.40f);
    }
}

void DialogRagdollEditor::AddHeadCollider()
{
    if (headObject->GetComponent(SphereCollider::COMPONENT_TYPE))
        headObject->RemoveComponent(headObject->GetComponent(SphereCollider::COMPONENT_TYPE));

    float radius = larmObject->_getDerivedPosition().distance(rarmObject->_getDerivedPosition());
    radius /= 4.0f;

    SphereCollider * sphere = new SphereCollider(headObject);
    headObject->AddComponent(sphere);
    sphere->SetRadius(radius);
    Vector3 center = Vector3::ZERO;

    int direction;
    float distance;

    CalculateDirection(Mathf::inverseTransformPoint(headObject, pelvisObject->_getDerivedPosition()), direction, distance);

    if (distance > 0)
        center[direction] = -radius;
    else
        center[direction] = radius;

    sphere->SetOffset(center);
}