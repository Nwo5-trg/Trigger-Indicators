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
        CCDrawNode* drawNode;
        CCDrawNode* drawExtraNode;
        int lastObjectCount;
        int maxClusterObjectCheck;
        float lineWidth;
        float lineAlpha;
        float maxXDif;
        float maxYDif;
        float clusterLimit;
        bool indicatorUpdateLock = true;
        bool triggerOnlyMode;
        bool useClusters;
        bool drawIndividualLines;
        std::unordered_set<int> groupBlacklist;
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
            indicatorRenderLayerExtra->setZOrder(1500);
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

        this->scheduleOnce(schedule_selector(Editor::unlockIndicatorUpdates), 0);
        fields->indicatorUpdateLock = true;

        auto drawNode = fields->drawNode;
        if (drawNode) drawNode->clear();
        auto drawExtraNode = fields->drawExtraNode;
        if (drawExtraNode) drawExtraNode->clear();
        auto lineWidth = fields->lineWidth;
        auto lineAlpha = fields->lineAlpha;
        auto maxXDif = fields->maxXDif;
        auto maxYDif = fields->maxYDif;
        auto triggerOnly = fields->triggerOnlyMode;
        auto useClusters = fields->useClusters;
        auto drawIndividualLines = fields->drawIndividualLines;
        auto clusterLimit = fields->clusterLimit;
        auto maxClusterObjectCheck = fields->maxClusterObjectCheck;

        for (auto trigger : CCArrayExt<EffectGameObject*>(m_drawGridLayer->m_effectGameObjects)) {
            if (!trigger) continue;
            auto triggerPos = trigger->getPosition();
            auto triggerData = getTriggerData(trigger);
            std::vector<GameObject*> groupObjects;
            if (!(fields->groupBlacklist.contains(triggerData.group))) if (auto array = static_cast<CCArray*>(m_groupDict->objectForKey(triggerData.group))) for (auto obj : CCArrayExt<GameObject*>(array)) groupObjects.push_back(obj);
            if (!(fields->groupBlacklist.contains(triggerData.centerGroup))) if (auto array = static_cast<CCArray*>(m_groupDict->objectForKey(triggerData.centerGroup))) for (auto obj : CCArrayExt<GameObject*>(array)) groupObjects.push_back(obj); // will eventually be moved to a new vector but for now its fine
            if (trigger->m_objectID == 3607 || trigger->m_objectID == 2068) {
                if (auto chanceTrigger = typeinfo_cast<ChanceTriggerGameObject*>(trigger)) {
                    for (auto& chanceObj : chanceTrigger->m_chanceObjects) {
                        if (auto array = static_cast<CCArray*>(m_groupDict->objectForKey(chanceObj.m_groupID))) for (auto obj : CCArrayExt<GameObject*>(array)) groupObjects.push_back(obj);
                    }
                } 
            }

            std::erase_if(groupObjects, [&] (GameObject* obj) { // did not know this existed
                auto objPos = obj->getPosition();
                if (triggerOnly && !isTrigger(obj, false)) return true;
                if (obj->m_isSelected || trigger->m_isSelected) return false;
                return !((std::abs(triggerPos.x - objPos.x) < maxXDif && std::abs(triggerPos.y - objPos.y) < maxYDif));
            });
            
            if (groupObjects.size() != 0) {
                if (drawIndividualLines) { // draw lines individually, temp
                    for (auto obj : groupObjects) drawLine(trigger, obj, drawNode, drawExtraNode, ccp(0.0f, 0.0f), CCRect(0.0f, 0.0f, 0.0f, 0.0f), lineWidth, lineAlpha);
                } else {
                    std::vector<GameObject*> centerVector;
                    for (auto obj : groupObjects) {
                        if (isTrigger(obj, true)) {
                            drawLine(trigger, obj, drawNode, drawExtraNode, ccp(0.0f, 0.0f), CCRect(0.0f, 0.0f, 0.0f, 0.0f), lineWidth, lineAlpha);
                        } else centerVector.push_back(obj);
                    }
                    if (centerVector.size() != 0) {
                        if (useClusters && centerVector.size() <= maxClusterObjectCheck) {
                            auto clustersVector = getClusters(centerVector, clusterLimit);
                            for (auto currentCluster : clustersVector) {
                                auto minMaxRect = getMinMaxRect(currentCluster);
                                CCPoint center = ccp(minMaxRect.origin.x + (minMaxRect.size.width - minMaxRect.origin.x), minMaxRect.origin.y + (minMaxRect.size.height - minMaxRect.origin.y));
                                drawLine(trigger, currentCluster[0], drawNode, drawExtraNode, currentCluster.size() > 1 ? center : ccp(0.0f, 0.0f), minMaxRect, lineWidth, lineAlpha);
                            }
                        } else {
                            auto minMaxRect = getMinMaxRect(centerVector);
                            CCPoint center = ccp(minMaxRect.origin.x + (minMaxRect.size.width - minMaxRect.origin.x), minMaxRect.origin.y + (minMaxRect.size.height - minMaxRect.origin.y));
                            drawLine(trigger, centerVector[0], drawNode, drawExtraNode, centerVector.size() > 1 ? center : ccp(0.0f, 0.0f), minMaxRect, lineWidth, lineAlpha);
                        }
                    }
                }
            }
        }
    }

    void drawLine(EffectGameObject* trigger, GameObject* obj, CCDrawNode* drawNode, CCDrawNode* drawExtraNode, CCPoint objOverride, CCRect minMaxRect, float width, float alpha) {
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

        if (isTrigger(obj, true)) {
            auto triggerScale = trigger->getScale();
            auto objScale = obj->getScale();
            drawNode->drawSegment(ccp(triggerPos.x + (10.0f * triggerScale), triggerPos.y - (5.0f * triggerScale)), ccp(objPos.x - (10.0f * objScale), objPos.y - (5.0f * objScale)), width, col);

            drawExtraNode->drawPolygon((CCPoint[3]){ccp(triggerPos.x + (8.0f * triggerScale), triggerPos.y - (2.5f * triggerScale)), ccp(triggerPos.x + (8.0f * triggerScale), triggerPos.y - (7.5f * triggerScale)),
            ccp(triggerPos.x + (12.0f * triggerScale), triggerPos.y - (5.0f * triggerScale))}, 3, ccColor4F{1.0f, 1.0f, 1.0f, 1.0f * layerAlphaMultiplier}, 0.45f * triggerScale, ccColor4F{0.0f, 0.0f, 0.0f, 1.0f * layerAlphaMultiplier});
            drawExtraNode->drawCircle(ccp(objPos.x - (10.0f * objScale), objPos.y - (5.0f * objScale)), 2.0f * objScale, ccColor4F{1.0f, 1.0f, 1.0f, 1.0f * layerAlphaMultiplier}, 0.5f * objScale, ccColor4F{0.0f, 0.0f, 0.0f, 1.0f * layerAlphaMultiplier}, 32);
            return;
        }
        if (objOverride != ccp(0.0f, 0.0f)) {
            auto lowestPoint = ccp(minMaxRect.origin.x, minMaxRect.origin.y);
            auto highestPoint = ccp(minMaxRect.size.width, minMaxRect.size.height);
            lowestPoint = ccp(lowestPoint.x - (15.0f + width), lowestPoint.y - (15.0f + width));
            highestPoint = ccp(highestPoint.x + (15.0f + width), highestPoint.y + (15.0f + width));

            drawNode->drawRect(lowestPoint, highestPoint, ccColor4F{0.0f, 0.0f, 0.0f, 0.0f}, width, col);
            // ccp(lowestPoint.x + ((highestPoint.x - lowestPoint.x) / 2.0f), highestPoint.y + (15.0f + width))
            drawNode->drawSegment(triggerPos, getLineCut(triggerPos, lowestPoint, highestPoint), width, col);
        } 
        else drawNode->drawSegment(triggerPos, objPos, 1.0f, col);
    }

    void updateSettingValues(bool updateIndicators) {
        m_fields->lineWidth = mod->getSettingValue<double>("line-width");
        m_fields->lineAlpha = mod->getSettingValue<double>("line-opacity-multiplier");
        m_fields->maxXDif = mod->getSettingValue<double>("max-x-distance");
        m_fields->maxYDif = mod->getSettingValue<double>("max-y-distance");
        m_fields->clusterLimit = mod->getSettingValue<double>("cluster-limit");
        m_fields->maxClusterObjectCheck = mod->getSettingValue<int64_t>("max-cluster-object-check");
        m_fields->triggerOnlyMode = mod->getSettingValue<bool>("enable-trigger-only");
        m_fields->useClusters =  mod->getSettingValue<bool>("enable-clustering");
        m_fields->drawIndividualLines = mod->getSettingValue<bool>("enable-draw-individual-lines");
        m_fields->groupBlacklist = getBlacklistedGroups();
        if (updateIndicators) updateAllIndicators();
    }

    ccColor4F getColorFromID(int id, bool multiplyAlpha, float alpha) {
        if (auto entry = colorMap.find(id); entry != colorMap.end()) {
            if (multiplyAlpha) return ccColor4F{entry->second.r, entry->second.g, entry->second.b, entry->second.a * (multiplyAlpha ? alpha : 1.0f)};
            return entry->second;
        }
        return ccColor4F{1.0f, 1.0f, 1.0f, 1.0f * (multiplyAlpha ? alpha : 1.0f)};
    }

    bool isTrigger(GameObject* obj, bool useBlacklist) {
        if (!obj) return false;
        if (auto objAsTrigger = typeinfo_cast<EffectGameObject*>(obj)) {
            if (objAsTrigger->m_isTrigger && (useBlacklist ? (!triggerTypeBlacklist.contains(obj->m_objectID)) : true)) return true;
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

    CCRect getMinMaxRect (std::vector<GameObject*> objs) {
        CCRect rect = CCRect(FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (auto obj : objs) {
            auto pos = obj->getPosition();
            if (pos.x < rect.origin.x) rect.origin.x = pos.x;
            if (pos.y < rect.origin.y) rect.origin.y = pos.y;
            if (pos.x > rect.size.width) rect.size.width = pos.x;
            if (pos.y > rect.size.height) rect.size.height = pos.y;
        }
        return rect;
    }

    CCPoint getLineCut (CCPoint triggerPos, CCPoint lowestPoint, CCPoint highestPoint) { // thanks chatgpt for explaing me this shit for like an hour :sob:
        CCPoint centerPos = ccp(lowestPoint.x + ((highestPoint.x - lowestPoint.x) / 2), lowestPoint.y + ((highestPoint.y - lowestPoint.y) / 2));
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

    std::vector<std::vector<GameObject*>> getClusters(std::vector<GameObject*> objs, float distance) {
        std::unordered_map<GameObject*, int> clusterMap;
        std::vector<std::vector<GameObject*>> clusters;
        for (auto obj : objs) {
            auto pos = obj->getPosition();
            auto objCluster = -1;
            clusterMap.insert({obj, objCluster});
            for (auto neighbourObj : objs) {
                auto neighbourPos = neighbourObj->getPosition();
                if (neighbourObj == obj) {
                    if (objCluster == -1) {
                        objCluster = clusters.size();
                        clusters.emplace_back(std::vector<GameObject*>{obj});
                        clusterMap[obj] = objCluster;
                    }
                    continue;
                }
                if (std::abs(pos.x - neighbourPos.x) <= distance && std::abs(pos.y - neighbourPos.y) <= distance) {
                    if (clusters.size() != 0 && clusterMap.contains(neighbourObj)) {
                        auto neighbourObjCluster = clusterMap[neighbourObj];
                        if (objCluster == -1) {
                            clusters[neighbourObjCluster].push_back(obj);
                            objCluster = neighbourObjCluster;   
                            clusterMap[obj] = neighbourObjCluster;
                        } else if (neighbourObjCluster != objCluster) {
                            auto& neighbourObjClusterVector = clusters[neighbourObjCluster];
                            for (auto currentObj : clusters[objCluster]) {
                                neighbourObjClusterVector.push_back(currentObj);
                                clusterMap[currentObj] = neighbourObjCluster;
                            }
                            clusters.erase(clusters.begin() + objCluster);
                            objCluster = neighbourObjCluster;
                        }
                    } else {
                        if (objCluster == -1) {
                            objCluster = clusters.size();
                            clusters.emplace_back(std::vector<GameObject*>{obj, neighbourObj});
                            clusterMap.insert({neighbourObj, objCluster});
                            clusterMap[obj] = objCluster;  
                        } else {
                            clusters[objCluster].push_back(neighbourObj);
                            clusterMap.insert({neighbourObj, objCluster});
                        }
                    }
                }
            }
        }
        return clusters;
        //return std::vector<std::vector<GameObject*>>();
    }
    
    void unlockIndicatorUpdates(float dt) {
        m_fields->indicatorUpdateLock = false;
    }
    
    void updateDebugDraw() { // this works surprisingly well lol
        LevelEditorLayer::updateDebugDraw();
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

    void selectObject(GameObject* p0, bool p1) { //test with update buttons to replace these and make undo/redo work
        EditorUI::selectObject(p0, p1);
        this->scheduleOnce(schedule_selector(EditUI::delayedUpdateAllIndicators), 0);
    }

    void selectObjects(CCArray* p0, bool p1) { //test with update buttons to replace these and make undo/redo work
        EditorUI::selectObjects(p0, p1);
        this->scheduleOnce(schedule_selector(EditUI::delayedUpdateAllIndicators), 0);
    }

    void deselectObject(GameObject* p0) {
        EditorUI::deselectObject(p0);
        this->scheduleOnce(schedule_selector(EditUI::delayedUpdateAllIndicators), 0);
    }

    void deselectAll() {
        EditorUI::deselectAll();
        this->scheduleOnce(schedule_selector(EditUI::delayedUpdateAllIndicators), 0);
    }

    void moveObject(GameObject* p0, CCPoint p1) {
        EditorUI::moveObject(p0, p1);
        if (!m_fields->updateMoveLock) {
            m_fields->updateMoveLock = true;
            this->scheduleOnce(schedule_selector(EditUI::delayedUpdateAllIndicators), 0);
        }
    }

    void delayedUpdateAllIndicators(float dt) { // so moving crazy amounts of objects doesnt make the game cry, and also some of these functions dont update somehow wtf
        static_cast<Editor*>(LevelEditorLayer::get())->updateAllIndicators();
        m_fields->updateMoveLock = false;
    }
};

class $modify(TriggerPopup, SetupTriggerPopup) { //workaround cuz i cant find the function for "effectgameobjectchangedthefuckinggroupsitactivatesorsomethingidk"
    void onClose(CCObject* sender) {
        SetupTriggerPopup::onClose(sender);
        if (auto editor = static_cast<Editor*>(LevelEditorLayer::get())) editor->updateAllIndicators();
    }
};