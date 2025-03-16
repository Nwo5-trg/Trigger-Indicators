#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

std::unordered_set<int> triggerLineTypeBlacklist {
    1615, 1815, 1816, 3609, 3640, 
};

std::unordered_set<int> triggerTypeBlacklist { // to be expanded btw
    1615, 1815, 1816, 3609, 3640, 
};

// thank you chatgpt for saving me like an hour on this
std::unordered_map<int, ccColor4F> colorMap = {
    {901, {1, 0, 1, 1}}, {3006, {1, 0, 1, 1}}, {3011, {1, 0, 1, 1}},  
    {1616, {0.639, 0, 0.337, 1}}, {1006, {1, 1, 0, 1}}, {3010, {1, 1, 0, 1}},  
    {3015, {1, 1, 0, 1}}, {3617, {1, 1, 0, 1}}, {3620, {1, 1, 0, 1}},  
    {2015, {1, 1, 0, 1}}, {1007, {0, 1, 1, 1}}, {3009, {0, 1, 1, 1}},  
    {3014, {0, 1, 1, 1}}, {3615, {0, 1, 1, 1}}, {3619, {0, 1, 1, 1}},  
    {2066, {0, 1, 1, 1}}, {1049, {1, 0.247, 0.247, 1}}, {1268, {0.137, 0.8, 0.498, 1}},  
    {1346, {0.498, 0.498, 1, 1}}, {3007, {0.498, 0.498, 1, 1}}, {3012, {0.498, 0.498, 1, 1}},  
    {1913, {0.498, 0.498, 1, 1}}, {2067, {0.247, 0.749, 1, 1}}, {3008, {0.247, 0.749, 1, 1}},  
    {3013, {0.247, 0.749, 1, 1}}, {2062, {0.247, 0.749, 1, 1}}, {1347, {1, 0.498, 0.498, 1}},  
    {1914, {1, 0.498, 0.498, 1}}, {1585, {1, 0.717, 0, 1}}, {1814, {1, 1, 0.498, 1}},  
    {3016, {0.8, 1, 0.78, 1}}, {3660, {0.8, 1, 0.78, 1}}, {3661, {0.8, 1, 0.78, 1}},  
    {1595, {0, 0.537, 0.698, 1}}, {1611, {1, 0.717, 0.988, 1}}, {1811, {1, 0.498, 1, 1}},  
    {2999, {1, 0.498, 1, 1}}, {3612, {1, 0.498, 1, 1}}, {1817, {1, 0.427, 0, 1}},  
    {3618, {1, 0.392, 0, 1}}, {1912, {0.247, 0.498, 1, 1}}, {2068, {0.647, 0.368, 1, 1}},  
    {3607, {0.647, 0.368, 1, 1}}, {3608, {0.588, 0.588, 0.588, 1}}, {3602, {1, 1, 1, 1}},  
    {3603, {1, 1, 1, 1}}, {3604, {1, 1, 1, 1}}, {1615, {1, 1, 1, 1}},  
    {3613, {1, 1, 1, 1}}, {3662, {1, 1, 1, 1}}, {3640, {1, 1, 1, 0.471}},  
    {1816, {1, 1, 1, 0.471}}, {3643, {1, 1, 1, 0.471}}, {1935, {0.6, 0.498, 0.6, 1}},  
    {1932, {0.396, 1, 1, 1}}, {3606, {0.498, 1, 1, 1}}, {1815, {0.325, 0.259, 0.98, 1}},  
    {3609, {0.325, 0.749, 1, 1}}, {1812, {0.8, 0.396, 0.396, 1}}, {3600, {0, 0, 0, 1}},  
    {3022, {0.408, 0.886, 1, 1}}, {2902, {0.408, 0.886, 1, 1}}, {2905, {0.498, 0.498, 0.498, 0.498}},  
    {2907, {0.498, 0.498, 0.498, 0.498}}, {2913, {0.498, 0.498, 0.498, 0.498}}, {2915, {0.498, 0.498, 0.498, 0.498}},  
    {2916, {0.498, 0.498, 0.498, 0.498}}  
};


class $modify(Editor, LevelEditorLayer) {
    struct TriggerData {
        EffectGameObject* obj;
        int group, centerGroup, modCenterGroup, rotationTargetGroup; // apparently u can do this but idk if its the same for fields
    };

    struct Fields {
        CCLayer* indicatorRenderLayer;
        CCLayer* indicatorRenderLayerExtra;
        CCLayer* batchLayer;
        std::vector<TriggerData> triggers;
        std::unordered_set<int> groupBlacklist;
        CCDrawNode* drawNode;
        CCDrawNode* drawExtraNode;
        float lineWidth = mod->getSettingValue<double>("line-width");;
        float lineAlpha = mod->getSettingValue<double>("line-opacity-multiplier");;
        float maxXDif = mod->getSettingValue<double>("max-x-distance");
        float maxYDif = mod->getSettingValue<double>("max-y-distance");
        bool indicatorUpdateLock = true; // stop indicators updating like a million times on editor open
        bool triggerOnlyMode = false;
        bool groupBlacklistEnabled = mod->getSettingValue<bool>("enable-group-blacklist");
    };

    bool init(GJGameLevel* p0, bool p1) {	
        if (!LevelEditorLayer::init(p0, p1)) return false;
            m_fields->groupBlacklist = getBlacklistedGroups();

            if (auto shaderLayer = this->getChildByType<ShaderLayer>(0)) m_fields->batchLayer = shaderLayer->getChildByType<CCNode>(1)->getChildByType<CCLayer>(0);
            else m_fields->batchLayer = this->getChildByType<CCNode>(1)->getChildByType<CCLayer>(0);

            auto indicatorRenderLayer = CCLayer::create();
            m_fields->indicatorRenderLayer = indicatorRenderLayer;
            indicatorRenderLayer->setPosition(ccp(0, 0));
            indicatorRenderLayer->setZOrder(-1481);
            indicatorRenderLayer->setID("indicator-render-layer");
            m_fields->batchLayer->addChild(indicatorRenderLayer);

            auto indicatorRenderLayerExtra = CCLayer::create();
            m_fields->indicatorRenderLayerExtra = indicatorRenderLayerExtra;
            indicatorRenderLayerExtra->setPosition(ccp(0, 0));
            indicatorRenderLayerExtra->setZOrder(9998);
            indicatorRenderLayerExtra->setID("indicator-render-layer-extra");
            m_fields->batchLayer->addChild(indicatorRenderLayerExtra);

            auto drawNode = CCDrawNode::create();
            m_fields->drawNode = drawNode;
            m_fields->drawNode->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA}); //remind me to add this to better editor trail
            indicatorRenderLayer->addChild(drawNode);
            drawNode->setPosition(ccp(0, 0));

            auto drawExtraNode = CCDrawNode::create();
            m_fields->drawExtraNode = drawExtraNode;
            // m_fields->drawExtraNode->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
            indicatorRenderLayerExtra->addChild(drawExtraNode);
            drawNode->setPosition(ccp(0, 0));

            auto objs = this->m_objects;
            auto fields = m_fields.self();
            for (int i = 0; i < objs->count(); i++) {
                if (auto obj = typeinfo_cast<EffectGameObject*>(objs->objectAtIndex(i))) {
                    if (obj->m_isTrigger && !triggerTypeBlacklist.contains(obj->m_objectID)) {
                        fields->triggers.emplace_back(getData(obj));
                    }
                }
            }
            m_fields->indicatorUpdateLock = false;
            updateAllIndicators();
        return true;
    }

    void updateAllIndicators() {
        auto fields = m_fields.self();

        if (fields->indicatorUpdateLock) return;

        auto drawNode = fields->drawNode;
        if (drawNode) drawNode->clear();
        auto drawExtraNode = fields->drawExtraNode;
        if (drawExtraNode) drawExtraNode->clear();
        auto lineWidth = fields->lineWidth;
        auto lineAlpha = fields->lineAlpha;
        auto maxXDif = fields->maxXDif;
        auto maxYDif = fields->maxYDif;

       auto selectedObj = EditorUI::get()->getSelectedObjects()->objectAtIndex(0);

        for (auto& trigger : fields->triggers) {
            auto triggerObj = trigger.obj;
            trigger = getData(triggerObj);
            CCArray groupObjects;
            if (fields->groupBlacklist.contains(trigger.group)) continue;
            for (int i = 0; i < m_objects->count(); i++) {
                auto obj = static_cast<GameObject*>(m_objects->objectAtIndex(i));
                if (fields->triggerOnlyMode) {
                    if (auto objAsTrigger = typeinfo_cast<EffectGameObject*>(obj)) {
                        if (!(objAsTrigger->m_isTrigger && !triggerTypeBlacklist.contains(obj->m_objectID))) continue;
                    } else continue;
                }
                if (!obj || !obj->m_groups) continue; // gotta test whats needed and what isnt
                if ((std::abs(triggerObj->getPosition().x - obj->getPosition().x) < maxXDif && std::abs(triggerObj->getPosition().y - obj->getPosition().y) < maxYDif) || selectedObj == triggerObj) {
                    auto& groups = *obj->m_groups; //according to gpt-san not using a pointer is better so
                    if (groups.empty()) continue;
                    if ((trigger.group != 0 && std::find(groups.begin(), groups.end(), trigger.group) != groups.end()) || 
                    (trigger.centerGroup != 0 && std::find(groups.begin(), groups.end(), trigger.centerGroup) != groups.end()) ||
                    (trigger.modCenterGroup != 0 && std::find(groups.begin(), groups.end(), trigger.modCenterGroup) != groups.end()) || 
                    (trigger.rotationTargetGroup != 0 && std::find(groups.begin(), groups.end(), trigger.rotationTargetGroup) != groups.end())) groupObjects.addObject(obj);
                }
            }
            if (groupObjects.count() != 0) {
                if (true != true) {
                    for (int i = 0; i < groupObjects.count(); i++) {
                        drawLine(triggerObj, static_cast<GameObject*>(groupObjects.objectAtIndex(i)), drawNode, drawExtraNode, ccp(0, 0), ccp(0, 0), ccp(0, 0), lineWidth, lineAlpha);
                    }
                } else {
                    CCArray centerArray;
                    for (int i = 0; i < groupObjects.count(); i++) {
                        if (auto obj = typeinfo_cast<EffectGameObject*>(groupObjects.objectAtIndex(i))) {
                            drawLine(triggerObj, obj, drawNode, drawExtraNode, ccp(0, 0), ccp(0, 0), ccp(0, 0), lineWidth, lineAlpha);
                        } else centerArray.addObject(groupObjects.objectAtIndex(i));
                    }
                    if (centerArray.count() != 0) {
                        CCPoint center = EditorUI::get()->getGroupCenter(&centerArray, true);
                        drawLine(triggerObj, centerArray.count() > 1 ? nullptr : static_cast<GameObject*>(centerArray.objectAtIndex(0)), drawNode, drawExtraNode, 
                        centerArray.count() > 1 ? (EditorUI::get()->getGroupCenter(&centerArray, true)) : ccp(0, 0), getLowestPoint(&centerArray), getHighestPoint(&centerArray), lineWidth, lineAlpha);
                    }
                }
            }
        }
    }   
    
    void drawLine(GameObject* trigger, GameObject* obj, CCDrawNode* drawNode, CCDrawNode* drawExtraNode, CCPoint objOverride, CCPoint lowestPoint, CCPoint highestPoint, float width, float alpha) {
        if (auto trigger2 = typeinfo_cast<EffectGameObject*>(obj)) {
            if (trigger2->m_isTrigger && !triggerLineTypeBlacklist.contains(obj->m_objectID)) {
                auto triggerPos = trigger->getPosition();
                auto objPos = obj->getPosition();
                drawNode->drawSegment(ccp(triggerPos.x + 10, triggerPos.y - 5), ccp(objPos.x - 10, objPos.y - 5), width, getColorFromID(trigger->m_objectID, true, alpha));
                drawExtraNode->drawPolygon((CCPoint[3]){ccp(triggerPos.x + 8, triggerPos.y - 2.5), ccp(triggerPos.x + 8, triggerPos.y - 7.5),
                ccp(triggerPos.x + 12, triggerPos.y - 5)}, 3, ccColor4F(1, 1, 1, 1), 0.45, ccColor4F(0, 0, 0, 1));
                drawExtraNode->drawCircle(ccp(objPos.x - 10, objPos.y - 5), 2, ccColor4F(1, 1, 1, 1), 0.5, ccColor4F(0, 0, 0, 1), 32);
                return;
            }
        }
        if (objOverride != ccp(0, 0)) {
            auto col = getColorFromID(trigger->m_objectID, true, alpha);
            drawNode->drawRect(ccp(lowestPoint.x - (15 + width), lowestPoint.y - (15 + width)), ccp(highestPoint.x + (15 + width), highestPoint.y + (15 + width)), ccColor4F(0, 0, 0, 0), width, col);
            drawNode->drawSegment(trigger->getPosition(), ccp(lowestPoint.x + ((highestPoint.x - lowestPoint.x) / 2), highestPoint.y + (15 + width)), width, col);
        } 
        else drawNode->drawSegment(trigger->getPosition(), obj->getPosition(), 1, getColorFromID(trigger->m_objectID, true, alpha));
    }

    ccColor4F getColorFromID(int id, bool multiplyAlpha, float alpha) {
        if (auto entry = colorMap.find(id); entry != colorMap.end()) {
            ccColor4F col = {entry->second.r, entry->second.g, entry->second.b, entry->second.a * alpha};
            if (multiplyAlpha) return col;
            return entry->second;
        }
        ccColor4F col = {1, 1, 1, 1 * alpha};
        return col;
    }

    std::unordered_set<int> getBlacklistedGroups() {
        std::unordered_set<int> blacklist;
        if (m_fields->groupBlacklistEnabled) {
            std::stringstream ss(mod->getSettingValue<std::string>("group-blacklist"));
            for (std::string str; std::getline(ss, str, ',');) {
                blacklist.insert(std::stoi(str));
            }
        }
        return blacklist;
    }

    TriggerData getData(EffectGameObject* obj) {
        return {obj, obj->m_targetGroupID, obj->m_centerGroupID,
        obj->m_targetModCenterID, obj->m_rotationTargetID};
    }

    CCPoint getLowestPoint (CCArray* objs) {
        CCPoint lowestPoint = ccp(FLT_MAX, FLT_MAX);
        for (int i = 0; i < objs->count(); i++) {
            auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));
            auto pos = obj->getPosition();
            if (pos.x < lowestPoint.x) {
                lowestPoint = ccp(pos.x, lowestPoint.y);
            }
            if (pos.y < lowestPoint.y) {
                lowestPoint = ccp(lowestPoint.x, pos.y);
            }
        }
        return lowestPoint;
    }

    CCPoint getHighestPoint (CCArray* objs) {
        CCPoint highestPoint = ccp(-FLT_MAX, -FLT_MAX);
        for (int i = 0; i < objs->count(); i++) {
            auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));
            auto pos = obj->getPosition();
            if (pos.x > highestPoint.x) {
                highestPoint = ccp(pos.x, highestPoint.y);
            }
            if (pos.y > highestPoint.y) {
                highestPoint = ccp(highestPoint.x, pos.y);
            }
        }
        return highestPoint;
    }

    GameObject* createObject(int p0, CCPoint p1, bool p2) {
        auto ret = LevelEditorLayer::createObject(p0, p1, p2);
        if (auto obj = typeinfo_cast<EffectGameObject*>(ret)) {
            if (obj->m_isTrigger && !obj->m_isStartPos) m_fields->triggers.emplace_back(getData(obj));
        }
        return ret;
    }

    void removeObject(GameObject* p0, bool p1) {
        if (auto obj = typeinfo_cast<EffectGameObject*>(p0)) {
            auto& triggers = m_fields->triggers;
            for (int i = 0; i < triggers.size(); i++) {
                if (triggers[i].obj == p0) {
                    triggers.erase(triggers.begin() + i);
                    break;
                }
            }
        }
        LevelEditorLayer::removeObject(p0, p1);
        updateAllIndicators();
    }
};

class $modify(EditUI, EditorUI) {
    struct Fields {
        bool updateMoveLock = false;
    };

    void selectObject(GameObject* p0, bool p1) { //test with update buttons to replace hese and make undo/redo work
        EditorUI::selectObject(p0, p1);
        static_cast<Editor*>(LevelEditorLayer::get())->updateAllIndicators();
    }

    void deselectObject(GameObject* p0) {
        EditorUI::deselectObject(p0);
        static_cast<Editor*>(LevelEditorLayer::get())->updateAllIndicators();
    }

    void deselectAll() {
        EditorUI::deselectAll();
        static_cast<Editor*>(LevelEditorLayer::get())->updateAllIndicators();
    }

    void moveObject(GameObject* p0, CCPoint p1) {
        EditorUI::moveObject(p0, p1);
        if (!m_fields->updateMoveLock) {
            m_fields->updateMoveLock = true;
            this->scheduleOnce(schedule_selector(EditUI::delayedUpdateAllIndicators), 0);
        }
    }

    void delayedUpdateAllIndicators(float dt) { // so moving crazy amounts of objects doesnt make the game cry
        static_cast<Editor*>(LevelEditorLayer::get())->updateAllIndicators();
        m_fields->updateMoveLock = false;
    }
};

class $modify(Obj, GameObject) {
    int addToGroup(int p0) {
        auto ret = GameObject::addToGroup(p0);
        if (auto editor = static_cast<Editor*>(LevelEditorLayer::get())) editor->updateAllIndicators();
        return ret;
    }

    void removeFromGroup(int p0) {
        GameObject::removeFromGroup(p0);
        if (auto editor = static_cast<Editor*>(LevelEditorLayer::get())) editor->updateAllIndicators();
    }
};

class $modify(TriggerPopup, SetupTriggerPopup) { //workaround cuz i cant find the function for "effectgameobjectchangedthefuckinggroupsitactivatesorsomethingidk"
    void onClose(CCObject* sender) {
        SetupTriggerPopup::onClose(sender);
        if (auto editor = static_cast<Editor*>(LevelEditorLayer::get())) editor->updateAllIndicators();
    }
};