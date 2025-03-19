#include <Geode/Geode.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

const std::unordered_set<int> triggerLineTypeBlacklist {
    1615, 1815, 1816, 3609, 3640
};

const std::unordered_set<int> triggerTypeBlacklist { // to be expanded btw
    1615, 1815, 1816, 3609, 3640
};

// thank you chatgpt for saving me like an hour on this
const std::unordered_map<int, ccColor4F> colorMap = {
    {901, {1.0f, 0.0f, 1.0f, 1.0f}}, {3006, {1.0f, 0.0f, 1.0f, 1.0f}}, {3011, {1.0f, 0.0f, 1.0f, 1.0f}},  
    {1616, {0.639f, 0.0f, 0.337f, 1.0f}}, {1006, {1.0f, 1.0f, 0.0f, 1.0f}}, {3010, {1.0f, 1.0f, 0.0f, 1.0f}},  
    {3015, {1.0f, 1.0f, 0.0f, 1.0f}}, {3617, {1.0f, 1.0f, 0.0f, 1.0f}}, {3620, {1.0f, 1.0f, 0.0f, 1.0f}},  
    {2015, {1.0f, 1.0f, 0.0f, 1.0f}}, {1007, {0.0f, 1.0f, 1.0f, 1.0f}}, {3009, {0.0f, 1.0f, 1.0f, 1.0f}},  
    {3014, {0.0f, 1.0f, 1.0f, 1.0f}}, {3615, {0.0f, 1.0f, 1.0f, 1.0f}}, {3619, {0.0f, 1.0f, 1.0f, 1.0f}},  
    {2066, {0.0f, 1.0f, 1.0f, 1.0f}}, {1049, {1.0f, 0.247f, 0.247f, 1.0f}}, {1268, {0.137f, 0.8f, 0.498f, 1.0f}},  
    {1346, {0.498f, 0.498f, 1.0f, 1.0f}}, {3007, {0.498f, 0.498f, 1.0f, 1.0f}}, {3012, {0.498f, 0.498f, 1.0f, 1.0f}},  
    {1913, {0.498f, 0.498f, 1.0f, 1.0f}}, {2067, {0.247f, 0.749f, 1.0f, 1.0f}}, {3008, {0.247f, 0.749f, 1.0f, 1.0f}},  
    {3013, {0.247f, 0.749f, 1.0f, 1.0f}}, {2062, {0.247f, 0.749f, 1.0f, 1.0f}}, {1347, {1.0f, 0.498f, 0.498f, 1.0f}},  
    {1914, {1.0f, 0.498f, 0.498f, 1.0f}}, {1585, {1.0f, 0.717f, 0.0f, 1.0f}}, {1814, {1.0f, 1.0f, 0.498f, 1.0f}},  
    {3016, {0.8f, 1.0f, 0.78f, 1.0f}}, {3660, {0.8f, 1.0f, 0.78f, 1.0f}}, {3661, {0.8f, 1.0f, 0.78f, 1.0f}},  
    {1595, {0.0f, 0.537f, 0.698f, 1.0f}}, {1611, {1.0f, 0.717f, 0.988f, 1.0f}}, {1811, {1.0f, 0.498f, 1.0f, 1.0f}},  
    {2999, {1.0f, 0.498f, 1.0f, 1.0f}}, {3612, {1.0f, 0.498f, 1.0f, 1.0f}}, {1817, {1.0f, 0.427f, 0.0f, 1.0f}},  
    {3618, {1.0f, 0.392f, 0.0f, 1.0f}}, {1912, {0.247f, 0.498f, 1.0f, 1.0f}}, {2068, {0.647f, 0.368f, 1.0f, 1.0f}},  
    {3607, {0.647f, 0.368f, 1.0f, 1.0f}}, {3608, {0.588f, 0.588f, 0.588f, 1.0f}}, {3602, {1.0f, 1.0f, 1.0f, 1.0f}},  
    {3603, {1.0f, 1.0f, 1.0f, 1.0f}}, {3604, {1.0f, 1.0f, 1.0f, 1.0f}}, {1615, {1.0f, 1.0f, 1.0f, 1.0f}},  
    {3613, {1.0f, 1.0f, 1.0f, 1.0f}}, {3662, {1.0f, 1.0f, 1.0f, 1.0f}}, {3640, {1.0f, 1.0f, 1.0f, 0.471f}},  
    {1816, {1.0f, 1.0f, 1.0f, 0.471f}}, {3643, {1.0f, 1.0f, 1.0f, 0.471f}}, {1935, {0.6f, 0.498f, 0.6f, 1.0f}},  
    {1932, {0.396f, 1.0f, 1.0f, 1.0f}}, {3606, {0.498f, 1.0f, 1.0f, 1.0f}}, {1815, {0.325f, 0.259f, 0.98f, 1.0f}},  
    {3609, {0.325f, 0.749f, 1.0f, 1.0f}}, {1812, {0.8f, 0.396f, 0.396f, 1.0f}}, {3600, {0.0f, 0.0f, 0.0f, 1.0f}},  
    {3022, {0.408f, 0.886f, 1.0f, 1.0f}}, {2902, {0.408f, 0.886f, 1.0f, 1.0f}}, {2905, {0.498f, 0.498f, 0.498f, 0.498f}},  
    {2907, {0.498f, 0.498f, 0.498f, 0.498f}}, {2913, {0.498f, 0.498f, 0.498f, 0.498f}}, {2915, {0.498f, 0.498f, 0.498f, 0.498f}},  
    {2916, {0.498f, 0.498f, 0.498f, 0.498f}}, {10001, {0.0f, 1.0f, 0.498f, 1.0f}}
};


class $modify(Editor, LevelEditorLayer) {
    struct TriggerData { // i should prolly get rid of this eventually but for now its fine
        EffectGameObject* obj;
        int group, centerGroup, modCenterGroup, rotationTargetGroup; // apparently u can do this but idk if its the same for fields
    };

    struct Fields {
        CCLayer* indicatorRenderLayer;
        CCLayer* indicatorRenderLayerExtra;
        CCLayer* batchLayer;
        std::unordered_set<int> groupBlacklist;
        CCDrawNode* drawNode;
        CCDrawNode* drawExtraNode;
        int lastObjectCount;
        float lineWidth;
        float lineAlpha;
        float maxXDif;
        float maxYDif;
        bool indicatorUpdateLock = true; // stop indicators updating like a million times on editor open
        bool triggerOnlyMode;
    };

    bool init(GJGameLevel* p0, bool p1) {	
        if (!LevelEditorLayer::init(p0, p1)) return false;
            m_fields->groupBlacklist = getBlacklistedGroups();

            if (auto shaderLayer = this->getChildByType<ShaderLayer>(0)) m_fields->batchLayer = shaderLayer->getChildByType<CCNode>(1)->getChildByType<CCLayer>(0);
            else m_fields->batchLayer = this->getChildByType<CCNode>(1)->getChildByType<CCLayer>(0);

            auto indicatorRenderLayer = CCLayer::create();
            m_fields->indicatorRenderLayer = indicatorRenderLayer;
            indicatorRenderLayer->setPosition(ccp(0.0f, 0.0f));
            indicatorRenderLayer->setZOrder(-1481);
            indicatorRenderLayer->setID("indicator-render-layer");
            m_fields->batchLayer->addChild(indicatorRenderLayer);

            auto indicatorRenderLayerExtra = CCLayer::create();
            m_fields->indicatorRenderLayerExtra = indicatorRenderLayerExtra;
            indicatorRenderLayerExtra->setPosition(ccp(0.0f, 0.0f));
            indicatorRenderLayerExtra->setZOrder(9998);
            indicatorRenderLayerExtra->setID("indicator-render-layer-extra");
            m_fields->batchLayer->addChild(indicatorRenderLayerExtra);

            auto drawNode = CCDrawNode::create();
            m_fields->drawNode = drawNode;
            m_fields->drawNode->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA}); //remind me to add this to better editor trail
            indicatorRenderLayer->addChild(drawNode);
            drawNode->setPosition(ccp(0.0f, 0.0f));

            auto drawExtraNode = CCDrawNode::create();
            m_fields->drawExtraNode = drawExtraNode;
            // m_fields->drawExtraNode->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
            indicatorRenderLayerExtra->addChild(drawExtraNode);
            drawNode->setPosition(ccp(0.0f, 0.0f));

            m_fields->indicatorUpdateLock = false;
            updateSettingValues(true);
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
        auto triggerOnly = fields->triggerOnlyMode;

        auto selectedObj = EditorUI::get()->getSelectedObjects()->objectAtIndex(0);

        for (int currentTrigger = 0; currentTrigger < m_drawGridLayer->m_effectGameObjects->count(); currentTrigger++) {
            auto trigger = static_cast<EffectGameObject*>(m_drawGridLayer->m_effectGameObjects->objectAtIndex(currentTrigger));
            if (!trigger) continue;
            auto triggerPos = trigger->getPosition();
            auto triggerData = getTriggerData(trigger);
            std::vector<CCObject*> groupObjects;
            if (!(fields->groupBlacklist.contains(triggerData.group))) if (auto array = static_cast<CCArray*>(m_groupDict->objectForKey(triggerData.group))) for (auto obj : CCArrayExt<CCObject*>(array)) groupObjects.push_back(obj);
            if (!(fields->groupBlacklist.contains(triggerData.centerGroup))) if (auto array = static_cast<CCArray*>(m_groupDict->objectForKey(triggerData.centerGroup))) for (auto obj : CCArrayExt<CCObject*>(array)) groupObjects.push_back(obj); // will eventually be moved to a new vector but for now its fine
            if (trigger->m_objectID == 3607 || trigger->m_objectID == 2068) {
                if (auto chanceTrigger = typeinfo_cast<ChanceTriggerGameObject*>(trigger)) {
                    for (auto& chanceObj : chanceTrigger->m_chanceObjects) {
                        if (auto array = static_cast<CCArray*>(m_groupDict->objectForKey(chanceObj.m_groupID))) for (auto obj : CCArrayExt<CCObject*>(array)) groupObjects.push_back(obj);
                    }
                } 
            }

            std::erase_if(groupObjects, [&] (CCObject* currentObj) { // did not know this existed
                auto obj = static_cast<GameObject*>(currentObj);
                auto objPos = obj->getPosition();
                if (triggerOnly && !isTrigger(obj)) return true;
                if (obj == selectedObj || trigger == selectedObj) return false;
                return !((std::abs(triggerPos.x - objPos.x) < maxXDif && std::abs(triggerPos.y - objPos.y) < maxYDif));
            });
            
            if (groupObjects.size() != 0) {
                if (true != true) { // draw lines individually, temp
                    for (auto& currentObj : groupObjects) drawLine(trigger, static_cast<GameObject*>(currentObj), drawNode, drawExtraNode, ccp(0.0f, 0.0f), ccp(0.0f, 0.0f), ccp(0.0f, 0.0f), lineWidth, lineAlpha);
                } else {
                    CCArray centerArray;
                    for (auto& currentObj : groupObjects) {
                        auto obj = static_cast<GameObject*>(currentObj);
                        if (isTrigger(obj)) {
                            drawLine(trigger, obj, drawNode, drawExtraNode, ccp(0.0f, 0.0f), ccp(0.0f, 0.0f), ccp(0.0f, 0.0f), lineWidth, lineAlpha);
                        } else centerArray.addObject(currentObj);
                    }
                    if (centerArray.count() != 0) {
                        CCPoint center = EditorUI::get()->getGroupCenter(&centerArray, true);
                        drawLine(trigger, static_cast<GameObject*>(centerArray.objectAtIndex(0)), drawNode, drawExtraNode, 
                        centerArray.count() > 1 ? (EditorUI::get()->getGroupCenter(&centerArray, true)) : ccp(0.0f, 0.0f), getLowestPoint(&centerArray), getHighestPoint(&centerArray), lineWidth, lineAlpha);
                    }
                }
            }
        }
    }   

    void drawLine(EffectGameObject* trigger, GameObject* obj, CCDrawNode* drawNode, CCDrawNode* drawExtraNode, CCPoint objOverride, CCPoint lowestPoint, CCPoint highestPoint, float width, float alpha) {
        auto triggerPos = trigger->getPosition();
        auto objPos = obj->getPosition();
        auto triggerID = trigger->m_objectID;
        if (triggerID == 1049 && trigger->m_activateGroup) triggerID = 10001;

        auto triggerLayer = trigger->m_editorLayer;
        auto objLayer = obj->m_editorLayer;
        auto triggerLayer2 = trigger->m_editorLayer2;
        auto objLayer2 = obj->m_editorLayer2;
        auto currentLayer = m_currentLayer;
        auto layerAlphaMultiplier = currentLayer == -1 || (((objLayer == currentLayer && objLayer == triggerLayer) || (objLayer2 == currentLayer && objLayer2 == triggerLayer2 && currentLayer != 0))) ? 1.0f : 0.5f;
        auto layerAlpha = alpha * layerAlphaMultiplier;
        
        auto col = getColorFromID(triggerID, true, layerAlpha);

        if (isTrigger(obj)) {
            auto triggerScale = trigger->getScale();
            auto objScale = obj->getScale();
            drawNode->drawSegment(ccp(triggerPos.x + (10.0f * triggerScale), triggerPos.y - (5.0f * triggerScale)), ccp(objPos.x - (10.0f * objScale), objPos.y - (5.0f * objScale)), width, col);
            // draw input/output indicators
            drawExtraNode->drawPolygon((CCPoint[3]){ccp(triggerPos.x + (8.0f * triggerScale), triggerPos.y - (2.5f * triggerScale)), ccp(triggerPos.x + (8.0f * triggerScale), triggerPos.y - (7.5f * triggerScale)),
            ccp(triggerPos.x + (12.0f * triggerScale), triggerPos.y - (5.0f * triggerScale))}, 3, ccColor4F{1.0f, 1.0f, 1.0f, 1.0f * layerAlphaMultiplier}, 0.45f * triggerScale, ccColor4F{0.0f, 0.0f, 0.0f, 1.0f * layerAlphaMultiplier});
            drawExtraNode->drawCircle(ccp(objPos.x - (10.0f * objScale), objPos.y - (5.0f * objScale)), 2.0f * objScale, ccColor4F{1.0f, 1.0f, 1.0f, 1.0f * layerAlphaMultiplier}, 0.5f * objScale, ccColor4F{0.0f, 0.0f, 0.0f, 1.0f * layerAlphaMultiplier}, 32);
            return;
        }
        if (objOverride != ccp(0.0f, 0.0f)) {
            lowestPoint = ccp(lowestPoint.x - (15.0f + width), lowestPoint.y - (15.0f + width));
            highestPoint = ccp(highestPoint.x + (15.0f + width), highestPoint.y + (15.0f + width));
            drawNode->drawRect(lowestPoint, highestPoint, ccColor4F{0.0f, 0.0f, 0.0f, 0.0f}, width, col);
            // ccp(lowestPoint.x + ((highestPoint.x - lowestPoint.x) / 2.0f), highestPoint.y + (15.0f + width))
            drawNode->drawSegment(triggerPos, getLineCut(triggerPos, objPos, lowestPoint, highestPoint), width, col);
        } 
        else drawNode->drawSegment(triggerPos, objPos, 1.0f, col);
    }

    void updateSettingValues(bool updateIndicators) {
        m_fields->lineWidth = mod->getSettingValue<double>("line-width");
        m_fields->lineAlpha = mod->getSettingValue<double>("line-opacity-multiplier");
        m_fields->maxXDif = mod->getSettingValue<double>("max-x-distance");
        m_fields->maxYDif = mod->getSettingValue<double>("max-y-distance");
        m_fields->triggerOnlyMode = mod->getSettingValue<bool>("enable-trigger-only");
        if (updateIndicators) updateAllIndicators();
    }

    ccColor4F getColorFromID(int id, bool multiplyAlpha, float alpha) {
        if (auto entry = colorMap.find(id); entry != colorMap.end()) {
            if (multiplyAlpha) return ccColor4F{entry->second.r, entry->second.g, entry->second.b, entry->second.a * (multiplyAlpha ? alpha : 1.0f)};
            return entry->second;
        }
        return ccColor4F{1.0f, 1.0f, 1.0f, 1.0f * (multiplyAlpha ? alpha : 1.0f)};
    }

    bool isTrigger(GameObject* obj) {
        if (!obj) return false;
        if (auto objAsTrigger = typeinfo_cast<EffectGameObject*>(obj)) {
            if (objAsTrigger->m_isTrigger && !triggerTypeBlacklist.contains(obj->m_objectID) && !objAsTrigger->m_isStartPos) return true;
        }
        return false;
    }

    std::unordered_set<int> getBlacklistedGroups() {
        std::unordered_set<int> blacklist;
        if (mod->getSettingValue<bool>("enable-group-blacklist")) {
            std::stringstream ss(mod->getSettingValue<std::string>("group-blacklist"));
            for (std::string str; std::getline(ss, str, ',');) {
                blacklist.insert(std::stoi(str));
            }
        }
        return blacklist;
    }

    TriggerData getTriggerData(EffectGameObject* obj) {
        return {obj, obj->m_targetGroupID == 0 ? -1 : obj->m_targetGroupID, obj->m_centerGroupID == 0 ? -1 : obj->m_centerGroupID, 
        obj->m_targetModCenterID == 0 ? -1 : obj->m_targetModCenterID, obj->m_rotationTargetID == 0 ? -1 : obj->m_rotationTargetID};
    }

    CCPoint getLowestPoint (CCArray* objs) {
        CCPoint lowestPoint = ccp(FLT_MAX, FLT_MAX);
        for (int i = 0; i < objs->count(); i++) {
            auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));
            auto pos = obj->getPosition();
            if (pos.x < lowestPoint.x) lowestPoint = ccp(pos.x, lowestPoint.y);
            if (pos.y < lowestPoint.y) lowestPoint = ccp(lowestPoint.x, pos.y);
        }
        return lowestPoint;
    }

    CCPoint getHighestPoint (CCArray* objs) {
        CCPoint highestPoint = ccp(-FLT_MAX, -FLT_MAX);
        for (int i = 0; i < objs->count(); i++) {
            auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));
            auto pos = obj->getPosition();
            if (pos.x > highestPoint.x) highestPoint = ccp(pos.x, highestPoint.y);
            if (pos.y > highestPoint.y) highestPoint = ccp(highestPoint.x, pos.y);
        }
        return highestPoint;
    }

    CCPoint getLineCut (CCPoint triggerPos, CCPoint centerPos, CCPoint lowestPoint, CCPoint highestPoint) { // thanks chatgpt for explaing me this shit for like an hour :sob:
        CCPoint direction = ccp(centerPos.x - triggerPos.x, centerPos.y - triggerPos.y);
        CCPoint intersection = centerPos;
        float cutAtT = 1.0f;

        for (float edge : {lowestPoint.x, highestPoint.x}) {
            if (direction.x != 0) {
                float t = (edge - triggerPos.x) / direction.x;
                float y = triggerPos.y + t * direction.y;
                if (t >= 0 && t <= 1 && y >= lowestPoint.y && y <= highestPoint.y && t < cutAtT) {
                    intersection = ccp(edge, y);
                    cutAtT = t;
                }
            }
        }

        for (float edge : {lowestPoint.y, highestPoint.y}) {
            if (direction.y != 0) {
                float t = (edge - triggerPos.y) / direction.y;
                float x = triggerPos.x + t * direction.x;
                if (t >= 0 && t <= 1 && x >= lowestPoint.x && x <= highestPoint.x && t < cutAtT) {
                    intersection = ccp(x, edge);
                    cutAtT = t;
                }
            }
        }

        return intersection;
    }

    void updateEditor(float p0) { // this works surprisingly well lol
        LevelEditorLayer::updateEditor(p0);
        auto& lastObjectCount = m_fields->lastObjectCount;
        auto objCount = m_objects->count();
        if (lastObjectCount != objCount) updateAllIndicators();
        lastObjectCount = objCount;
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