#include "Gameplay.h"
#include "ObjLoader.h"

/**
 * model init methods:
 * 1. InitMaterials
 * 2. InitCameras
 * 3. InitSnake
 * 4. initObjects
 */

void Gameplay::Init()
{
    //std::cout << "\nNEW TEXTURE COORDINATES\n" << getTextureCoords("data/zcylinder.obj") << std::endl;
    InitMaterials();
    InitSnake();
    generateViableEntities();
    spawnExtras();
    InitLevel();
    InitCoordsys();
    imGuiOverlay.leaderboard.Init(getResource("LEADERBOARD.txt"));
    //std::cout << "\nOG TEXTURE COORDINATES\n" << cyls[0].model->GetMesh(0)->data[0].textureCoords << std::endl;
}

void Gameplay::InitCoordsys() {
    // Creation of axis mesh
    Eigen::MatrixXd vertices(6, 3);
    vertices << -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, -1, 0, 0, 1;
    Eigen::MatrixXi faces(3, 2);
    faces << 0, 1, 2, 3, 4, 5;
    Eigen::MatrixXd vertexNormals = Eigen::MatrixXd::Ones(6, 3);
    Eigen::MatrixXd textureCoords = Eigen::MatrixXd::Ones(6, 2);
    std::shared_ptr<Mesh> coordsysMesh = std::make_shared<Mesh>("coordsys", vertices, faces, vertexNormals,
                                                                textureCoords);
    auto axis = Model::Create("Axis", coordsysMesh, basicMaterial);
    axis->mode = 1;
    axis->Scale(20);
    igl::AABB<Eigen::MatrixXd, 3> aabb;
    coordsys = {axis, 20.0, aabb};
    axis->isHidden = true;
}

void Gameplay::InitMaterials() {
    program = std::make_shared<Program>("shaders/basicShader");
    basicMaterial = std::make_shared<Material>("basicMaterial", program); // empty material
    basicMaterial->AddTexture(0, "textures/snake.png", 2);
    frameColor = std::make_shared<Material>("green", program, true);
    frameColor->AddTexture(0, "textures/grass.bmp", 2);
    collisionColor = std::make_shared<Material>("red", program);
    collisionColor->AddTexture(0, "textures/box0.bmp", 2);
    snakeShader = std::make_shared<Program>("shaders/basicShader");
    snakeSkin = std::make_shared<Material>("snakeSkin", snakeShader);
    snakeSkin->AddTexture(0, "textures/snake.png", 2);
    itemShader = std::make_shared<Program>("shaders/phongShader2");
    itemMaterial = std::make_shared<Material>("itemMaterial", itemShader);
    itemShader->name = "itemShader";
    enemyShader = std::make_shared<Program>("shaders/phongShader2");
    enemyMaterial = std::make_shared<Material>("enemyMaterial", enemyShader);
    enemyShader->name = "enemyShader";
    bonusShader = std::make_shared<Program>("shaders/phongShader2");
    bonusMaterial = std::make_shared<Material>("bonusMaterial", bonusShader);
    bonusShader->name = "bonusShader";
    }

void Gameplay::InitSnake() {
    //init cylinders
    auto cylMesh = IglLoader::MeshFromFiles("cyl_igl", "data/zCylinder.obj");
    float scaleFactor = 1.0f;
    igl::AABB<Eigen::MatrixXd, 3> cyl_aabb = CollisionDetection::InitAABB(cylMesh);
    for (int i = 0; i < numOfCyls; i++) {
        auto cylModel = ObjLoader::ModelFromObj("Cyl " + std::to_string(i), "data/zcylinder.obj", basicMaterial);
        cyls.push_back({cylModel, scaleFactor, cyl_aabb});
        CollisionDetection::InitCollisionModels(cyls[i], frameColor, collisionColor);
//        cyls[i].model->showFaces = false;
        if (!showCyls)
        {
            cyls[i].model->isHidden = true;
        }

        if (i == 0) // first axis and cylinder depend on scene's root
        {
            root->AddChild(cyls[0].model);
            cyls[0].model->Translate({0, 0, 0.8f});
            cyls[0].model->SetCenter(Eigen::Vector3f(0, 0, -0.8f));
        } else {
            cyls[i - 1].model->AddChild(cyls[i].model);
            //cyls[i].model->Scale(1, Axis::X);
            cyls[i].model->Translate(1.6f, Movable::Axis::Z);
            cyls[i].model->SetCenter(Eigen::Vector3f(0, 0, -0.8f));
        }
    }

    // init head
    auto headMesh = IglLoader::MeshFromFiles("head", "data/viperagabon.obj");
    auto headModel = Model::Create("head", headMesh, itemMaterial);
    headModel->Scale(0.3f);
    igl::AABB<Eigen::MatrixXd, 3> head_aabb = CollisionDetection::InitAABB(headMesh);
    head = {headModel, 1.0f, head_aabb};
    CollisionDetection::InitCollisionModels(head, frameColor, collisionColor);
    headModel->Scale(head.scaleFactor);
//    headModel->Rotate((float) -M_PI, Movable::Axis::Y);
    headModel->Translate(-1.6f, Movable::Axis::Z);
    headModel->showFaces = true;
    headModel->showWireframe = false;
    cyls[0].model->AddChild(headModel);

    if (useSnake)
    {
        // init snake
        auto snakeMesh = IglLoader::MeshFromFiles("snakeMesh", "data/snake3.obj");
        auto snakeModel = Model::Create("SNAKE", snakeMesh, snakeSkin);
        igl::AABB<Eigen::MatrixXd, 3> snake_aabb;
        snake = {snakeModel, 16.0f, snake_aabb};

        Eigen::MatrixXd V_uv;
        setUV(snake.model->GetMesh(0)->data[0].vertices, snake.model->GetMesh(0)->data[0].faces, V_uv);
        // create new mesh with UV
        std::shared_ptr<cg3d::Mesh> newMesh = std::make_shared<cg3d::Mesh>(snake.model->name,
                                                                           snake.model->GetMesh(0)->data[0].vertices,
                                                                           snake.model->GetMesh(0)->data[0].faces,
                                                                           snake.model->GetMesh(0)->data[0].vertexNormals,
                                                                           V_uv
        );
        // update snake mesh
        snake.model->SetMeshList({newMesh});
//        setAllUVs(snake.model->GetMesh(0)->data[0].vertices, snake.model->GetMesh(0)->data[0].faces, uv_vec);

        snakeSkinning.InitSkinning(snake, cyls);

        root->AddChild(snakeModel);

//        snake.model->showFaces = false;
//        snake.model->showWireframe = true;

//        Eigen::MatrixXd V_uv;
//        setUV(snake.model->GetMesh(0)->data[0].vertices, snake.model->GetMesh(0)->data[0].faces, V_uv);
//        // create new mesh with UV
//        std::shared_ptr<cg3d::Mesh> newMesh = std::make_shared<cg3d::Mesh>(snake.model->name,
//                                                                                snake.model->GetMesh(0)->data[0].vertices,
//                                                                                snake.model->GetMesh(0)->data[0].faces,
//                                                                                snake.model->GetMesh(0)->data[0].vertexNormals,
//                                                                                V_uv
//        );
//        // update snake mesh
//        snake.model->SetMeshList({newMesh});
    }

}

void Gameplay::generateViableEntities() {
    viableItems.push_back({"Bunny", "data/bunny.off", 10.0f, EntityType::ITEM, 1000, 100});
    viableItems.push_back({"Cheburashka", "data/cheburashka.off", 1.5f, EntityType::ITEM, 500, 80});
    viableItems.push_back({"Cow", "data/cow.off", 2.0f, EntityType::ITEM, 500, 80});
    //viableEnemies.push_back({"Screwdriver", "data/screwdriver.off", 10.0f, EntityType::ENEMY, -1000, 100}); not a good model
    viableEnemies.push_back({"Knight", "data/decimated-knight.off", 2.0f, EntityType::ENEMY, -500, 80});
    //viableEnemies.push_back({"Sword", "data/Sword01.off", 0.0005f, EntityType::ENEMY, -1000, 100});
    viableBonuses.push_back({"Apple", "data/Apple.off", 0.005f, EntityType::BONUS, 0, 100});
}

entity_data Gameplay::initEntity(Entity ent, std::shared_ptr<cg3d::Material> material, bool visible) {
    auto mesh = cg3d::IglLoader::MeshFromFiles("Entity_" + entityTypeToString(ent.type) + ent.name, ent.pathToMesh);
    auto model = cg3d::Model::Create("Entity_" + entityTypeToString(ent.type) + ent.name, mesh, material);
    igl::AABB<Eigen::MatrixXd, 3> aabb = CollisionDetection::InitAABB(mesh);
    model_data currentModel = {model, ent.scale, aabb};
    CollisionDetection::InitCollisionModels(currentModel, frameColor, collisionColor);
    if (visible) root->AddChild(model);
    model->Translate({0.0, 0.0, -5.0});
    model->Scale(ent.scale);
    //model->showFaces = false;
    model->showFaces = true;
    //model->showWireframe = true;
    model->showWireframe = false;
    entity_data currentEntity = {currentModel, time(nullptr),
                                 {ent.name, ent.pathToMesh, ent.scale, ent.type, ent.points,
                                  ent.lifeTime}};
    if (visible) entities.push_back(currentEntity);
    return currentEntity;
}

void Gameplay::randomizeTranlate(entity_data& entity)
{
    int x_value = getRandomNumberInRange(-MAP_SIZE, MAP_SIZE);
    int z_value = getRandomNumberInRange(-MAP_SIZE, MAP_SIZE);
    int y_value = getRandomNumberInRange(-MAP_SIZE, MAP_SIZE);
    entity.modelData.model->Translate({ (float)x_value, (float)y_value, (float)z_value });
}

void Gameplay::spawnEntity(int index, std::vector<Entity> &viableEntities) {
    if (index == -1)
        index = getRandomNumberInRange(0, (int)viableEntities.size());
    switch (viableEntities[index].type) {
        case EntityType::ENEMY:
            initEntity(viableEntities[index], enemyMaterial);
            currentEnemies++;
            break;
        case EntityType::ITEM:
            initEntity(viableEntities[index], itemMaterial);
            currentItems++;
            break;
        case EntityType::BONUS:
            initEntity(viableEntities[index], bonusMaterial);
            currentBonuses++;
            break;
    }
    randomizeTranlate(entities[entities.size() - 1]);
}

void Gameplay::spawnEntities(int amount, std::vector<Entity> &viableEntities) {
    while (amount > 0) {
        if (viableEntities[0].type == EntityType::ENEMY)
        {
            entity_data entityToSpawn = extraEnemies[0];
            extraEnemies.erase(extraEnemies.begin());
            randomizeTranlate(entityToSpawn);
            currentEnemies++;
            root->AddChild(entityToSpawn.modelData.model);
            entities.push_back(entityToSpawn);
        }
        else
            spawnEntity(-1, viableEntities);
        amount--;
    }
}

void Gameplay::spawnExtras()
{
    for (int i = 0; i < viableItems.size(); i++)
    {
        auto currentItem = initEntity(viableItems[i], itemMaterial, false);
        extraItems.push_back(currentItem);
    }
    for (int i = 0; i < 10; i++)
    {
        auto currentEnemy = initEntity(viableEnemies[i % viableEnemies.size()], enemyMaterial, false);
        extraEnemies.push_back(currentEnemy);
    }
}

void Gameplay::swapEntities(entity_data& entity, std::vector<entity_data> extras)
{
    entity_data entityToSpawn = extras[0];
    extras.erase(extras.begin());
    extras.push_back(entity);
    findAndDeleteEntity(entity);
    randomizeTranlate(entityToSpawn);
    root->AddChild(entityToSpawn.modelData.model);
    entities.push_back(entityToSpawn);
}

void Gameplay::replaceEntity(entity_data& entity)
{
    // If hit a bonus, just randomize it's position, we're not going to respawn it
    switch (entity.ent.type)
    {
    case EntityType::BONUS:
        randomizeTranlate(entity);
        return;
    case EntityType::ITEM:
        swapEntities(entity, extraItems);
        currentItems++;
        break;
    case EntityType::ENEMY: // theoretically should never happen as we reset once hit an enemy
        swapEntities(entity, extraEnemies);
        currentEnemies++;
        break;
    }
}

void Gameplay::clearEntities() {
    for (entity_data entity: entities) {
        root->RemoveChild(entity.modelData.model);
    }
    entities.clear();
}

void Gameplay::InitLevel() {
    int enemies, items = 10 - currentItems, bonuses = 2 - currentBonuses;
    switch (imGuiOverlay.currentLevel) {
        case 1:
            enemies = 3;
            break;
        case 2:
            enemies = 5;
            break;
        case 3:
            enemies = 7;
            break;
        default:
            enemies = 10;
    }
    enemies = enemies - currentEnemies;
    spawnEntities(items, viableItems);
    spawnEntities(enemies, viableEnemies);
    spawnEntities(bonuses, viableBonuses);
}

void Gameplay::UpdateScore(int score) 
{
    imGuiOverlay.currentScore += score;
    if (imGuiOverlay.currentScore < 0) imGuiOverlay.currentScore = 0;
    imGuiOverlay.currentScoreFormatted = imGuiOverlay.formatScore();
}

/**
 * need to add actions to be made when a collision of 2 objects detected, like:
 * 1. when the head of the snake collides with a "good" object, it should be eaten and gain score
 * 2. when the head of the snake collides with a "bad" object or itself, it should lower the score or end the game
 * 3. any other scenarios...
 */
void Gameplay::checkForCollision()
{
    // collision with the head and cylinders:
    for (int i = 1; i < cyls.size(); i++) {
        Eigen::AlignedBox3d box_camel, box_i;
        if (CollisionDetection::intersects(head, cyls[i], box_camel, box_i)) {
            imGuiOverlay.currentLives--;
            imGuiOverlay.died = true;
            if (imGuiOverlay.currentLives <= 0)
            {
                callPythonScript("scripts/play_sound.py", "audio/fatality.mp3", 3);
//                std::thread fatalityThread([&]() {
//                    std::system(getPyScript("scripts/play_sound.py", "audio/fatality.mp3", 3).c_str());
//                    });
//                fatalityThread.detach();
                Reset(true);
            } else {
                callPythonScript("scripts/play_sound.py", "audio/death_noise.mp3", 2);
//                std::thread deathThread([&]() {
//                    std::system(getPyScript("scripts/play_sound.py", "audio/death_noise.mp3", 2).c_str());
//                    });
//                deathThread.detach();
                Reset(false);
            }
        }
    }
    // collision with head and entities
    for (int i = 0; i < entities.size(); i++) {
        Eigen::AlignedBox3d box_camel, box_i;
        if (CollisionDetection::intersects(head, entities[i].modelData, box_camel, box_i))
        {
            HandleEntityCollision(i);
        }
    }

//    for (int i = 0; i < cyls.size(); i++) {
//        // search for collision between cylinder and cylinder
//        for (int j = i + 2; j < cyls.size(); j++) {
//            Eigen::AlignedBox3d box_i, box_j;
//            if (CollisionDetection::intersects(cyls[i], cyls[j], box_i, box_j)) {
//                imGuiOverlay.currentLives--;
//                imGuiOverlay.died = true;
//                if (imGuiOverlay.currentLives <= 0)
//                {
//                    Reset(true);
//                } else {
//                    Reset(false);
//                }
//            }
//        }
//        // search for collision between cylinders and entities:
//        for (int j = 0; j < entities.size(); j++) {
//            Eigen::AlignedBox3d box_i, box_j;
//            if (CollisionDetection::intersects(cyls[i], entities[j].modelData, box_i, box_j)) {
//                animate = false;
//                CollisionDetection::SetCollisionBox(cyls[i], box_i);
//                CollisionDetection::SetCollisionBox(entities[j].modelData, box_j);
//            }
//        }
//    }
}

void Gameplay::HandleEntityCollision(int i)
{
    switch (entities[i].ent.type) {
        case EntityType::ITEM:
            UpdateScore(entities[i].ent.points);
            if (shouldLevelUp())
            {
                callPythonScript("scripts/play_sound.py", "audio/wow.mp3", 5);
//                std::thread wowThread([&]() {
//                    std::system(getPyScript("scripts/play_sound.py", "audio/wow.mp3", 5).c_str());
//                    });
//                wowThread.detach();
                Reset(false);
            } else {
                callPythonScript("scripts/play_sound.py", "audio/ring_sound.mp3", 1);
//                std::thread ringThread([&]() {
//                    std::system(getPyScript("scripts/play_sound.py", "audio/ring_sound.mp3", 1).c_str());
//                    });
//                ringThread.detach();
                replaceEntity(entities[i]);
            }
            break;
        case EntityType::ENEMY:
            UpdateScore(entities[i].ent.points);
            imGuiOverlay.currentLives--;
            imGuiOverlay.died = true;
            if (imGuiOverlay.currentLives <= 0)
            {
                callPythonScript("scripts/play_sound.py", "audio/fatality.mp3", 2);
//                std::thread fatalityThread([&]() {
//                    std::system(getPyScript("scripts/play_sound.py", "audio/fatality.mp3", 3).c_str());
//                    });
//                fatalityThread.detach();
                Reset(true);
            } else {
                callPythonScript("scripts/play_sound.py", "audio/death_noise.mp3", 2);
//                std::thread deathThread([&]() {
//                    std::system(getPyScript("scripts/play_sound.py", "audio/death_noise.mp3", 2).c_str());
//                    });
//                deathThread.detach();
                Reset(false);
            }
            break;
        case EntityType::BONUS:
            replaceEntity(entities[i]);
            handleBonus();
            break;
    }
}

//void Gameplay::checkTimedOutEntities() {
//    for (int i = 0; i < entities.size(); i++) {
//        deleteEntityIfTimedOut(i);
//    }
//}

//void Gameplay::deleteEntityIfTimedOut(int index) {
//    entity_data entity = entities[index];
//    time_t now = time(nullptr);
//    if (now - entity.creationTime < entity.ent.lifeTime / imGuiOverlay.currentLevel)
//        return;
//    DeleteEntity(index);
//    // should insert new entity of the same type:
//    switch (entity.ent.type) {
//        case EntityType::ITEM:
//            spawnEntity(getRandomNumberInRange(0, (int) viableItems.size()), viableItems);
//            break;
//        case EntityType::ENEMY:
//            spawnEntity(getRandomNumberInRange(0, (int) viableEnemies.size()), viableEnemies);
//            break;
//        case EntityType::BONUS:
//            spawnEntity(getRandomNumberInRange(0, (int) viableBonuses.size()), viableBonuses);
//            break;
//    }
//}

void Gameplay::DeleteEntity(int index) {
    switch (entities[index].ent.type) {
        case EntityType::ENEMY:
            currentEnemies--;
            break;
        case EntityType::ITEM:
            currentItems--;
            break;
        case EntityType::BONUS:
            currentBonuses--;
            break;
    }
    root->RemoveChild(entities[index].modelData.model);
    std::vector<entity_data> newEntities;
    for (int i = 0; i < entities.size(); i++) {
        if (i != index) newEntities.push_back(entities[i]);
    }
    entities = newEntities;
}

void Gameplay::findAndDeleteEntity(entity_data& entity)
{
    int index = -1;
    for (int i = 0; i < entities.size(); i++)
    {
        auto currentEntityData = entities[i];
        bool createdAtTheSameTime = entity.creationTime == currentEntityData.creationTime;
        bool sameType = entity.ent.type == currentEntityData.ent.type;
        bool sameName = entity.ent.name == currentEntityData.ent.name;
        if (sameName && sameType && createdAtTheSameTime)
        {
            index = i;
            break;
        }
    }
    if (index == -1)
        std::cerr << "CANT HAPPEN" << std::endl;
    DeleteEntity(index);
}

/**
 * model delete methods:
 * 1. ResetSnake
 * 2. Reset
 */
void Gameplay::ResetSnake() {
    // reset all the cylinders
    for (int i = 0; i < cyls.size(); i++) {
        cyls[i].model->SetTout(Eigen::Affine3f::Identity());
        cyls[i].model->SetTin(Eigen::Affine3f::Identity());
        cyls[i].model->PropagateTransform();
        if (i == 0) // first axis and cylinder depend on scene's root
        {
            root->AddChild(cyls[0].model);
            cyls[0].model->Translate({0, 0, 0.8f});
            cyls[0].model->SetCenter(Eigen::Vector3f(0, 0, -0.8f));
        } else {
            cyls[i - 1].model->AddChild(cyls[i].model);
            //cyls[i].model->Scale(1, Axis::X);
            cyls[i].model->Translate(1.6f, Movable::Axis::Z);
            cyls[i].model->SetCenter(Eigen::Vector3f(0, 0, -0.8f));
        }
    }
    // reset head
    head.model->SetTout(Eigen::Affine3f::Identity());
    head.model->SetTin(Eigen::Affine3f::Identity());
    head.model->PropagateTransform();
    head.model->Scale(head.scaleFactor);
    head.model->Rotate((float) -M_PI, Movable::Axis::Y);
    head.model->Translate(-1.6f, Movable::Axis::Z);

    if (useSnake)
    {
        // reset snake model
        std::shared_ptr<cg3d::Mesh> deformedMesh = std::make_shared<cg3d::Mesh>(snake.model->name,
                                                                                snakeSkinning.V,
                                                                                snake.model->GetMesh(0)->data[0].faces,
                                                                                snake.model->GetMesh(0)->data[0].vertexNormals,
                                                                                snake.model->GetMesh(0)->data[0].textureCoords
        );
        // change snake mesh to the transformed one
        snake.model->SetMeshList({deformedMesh});
        snakeSkinning.InitTransformations(cyls);

//        snake.model->SetMeshList({deformedMesh});
//        snake.model->SetTout(Eigen::Affine3f::Identity());
//        snake.model->SetTin(Eigen::Affine3f::Identity());
//        snake.model->PropagateTransform();
//        snake.model->Translate(1.6f * (numOfCyls / 2) - 0.8f, Movable::Axis::Z);
//        snake.model->Scale(16.0f, Movable::Axis::Z);
//        snake.model->SetCenter(Eigen::Vector3f(0, 0, -0.8f));
    }

}

void Gameplay::Reset(bool mainMenu) {
    /*
        1. Delete the snake
        2. Delete all the entities
        3. Initiate the snake
        4. Delete the cameras
        5. Initialize cameras
        *6. Level should be initialized after call for start level so that
        *   entities won't pass away too quickly
    */
    animate = false;
    if (mainMenu) {
        imGuiOverlay.countdown = false;
        imGuiOverlay.countdownTimerEnd = 0;
        imGuiOverlay.accumulatedTime = 0;
    }
        //root->RemoveChild(snake.model); need to create a method to remove the snake
    else {
        imGuiOverlay.countdown = true;
        imGuiOverlay.countdownTimerEnd = 0;
        imGuiOverlay.accumulatedTime += time(nullptr) - imGuiOverlay.gameTimer;
    }
    ResetSnake();
    InitLevel();
    callResetCameras = true;
}

bool Gameplay::shouldLevelUp() {
    bool leveledUp = false;
    switch (imGuiOverlay.currentLevel) {
        case 1:
            if (imGuiOverlay.currentScore >= 3000) {
                leveledUp = true;
                imGuiOverlay.currentLevel++;
            }
            break;
        case 2:
            if (imGuiOverlay.currentScore >= 6000) {
                leveledUp = true;
                imGuiOverlay.currentLevel++;
            }
            break;
        case 3:
            if (imGuiOverlay.currentScore >= 9000) {
                leveledUp = true;
                imGuiOverlay.currentLevel++;
            }
            break;
        default:
            break;
    }
    imGuiOverlay.leveledUp = leveledUp;
    return leveledUp;
}

void Gameplay::updateGameplay()
{
//    checkTimedOutEntities();
    checkForCollision();
}

void Gameplay::handleBonus()
{
    int bonusIndex = getRandomNumberInRange(0, bonusPercentage.size());
    if (bonusPercentage[bonusIndex] == Bonus::LIFE)
    {
        imGuiOverlay.currentLives++;
        callPythonScript("scripts/play_sound.py", "audio/sheesh.mp3", 3);
//        std::thread sheeshThread([&]() {
//            std::system(getPyScript("scripts/play_sound.py", "audio/sheesh.mp3", 3).c_str());
//            });
//        sheeshThread.detach();
        return;
    }
    if (bonusPercentage[bonusIndex] == Bonus::POINTS)
    {
        UpdateScore(getRandomNumberInRange(0, 2000));
        if (shouldLevelUp())
        {
            callPythonScript("scripts/play_sound.py", "audio/wow.mp3", 5);
//            std::thread wowThread([&]() {
//                std::system(getPyScript("scripts/play_sound.py", "audio/wow.mp3", 5).c_str());
//                });
//            wowThread.detach();
            Reset(false);
        }
        else {
            callPythonScript("scripts/play_sound.py", "audio/ring_sound.mp3", 1);
//            std::thread ringThread([&]() {
//                std::system(getPyScript("scripts/play_sound.py", "audio/ring_sound.mp3", 1).c_str());
//                });
//            ringThread.detach();
        }
        return;
    }
    if (bonusPercentage[bonusIndex] == Bonus::SPEED_PLUS)
    {
        if (velocityVec.z() > -0.55f)
            velocityVec -= Eigen::Vector3f({ 0.0f, 0.0f, 0.1f });
        if (slerpFactor > 0.8f)
            slerpFactor -= 0.02f;
        callPythonScript("scripts/play_sound.py", "audio/neeoom.mp3", 3);
//        std::thread neeoomThread([&]() {
//            std::system(getPyScript("scripts/play_sound.py", "audio/neeoom.mp3", 3).c_str());
//            });
//        neeoomThread.detach();
        return;
    }
    if (bonusPercentage[bonusIndex] == Bonus::SPEED_MINUS)
    {
        callPythonScript("scripts/play_sound.py", "audio/bruh.mp3", 1);
//        std::thread slowThread([&]() {
//            std::system(getPyScript("scripts/play_sound.py", "audio/bruh.mp3", 1).c_str());
//            });
//        slowThread.detach();
        if (velocityVec.z() < -0.05f)
            velocityVec += Eigen::Vector3f({ 0.0f, 0.0f, 0.1f });
        if (slerpFactor < 1.0f)
            slerpFactor += 0.02f;
        return;
    }
}
Eigen::MatrixXd Gameplay::getTextureCoords(const char* filename)
{
    auto model = ObjLoader::ModelFromObj("Model", filename, basicMaterial);
    std::vector<Eigen::Vector2d> projectedVertices;
    for(int i = 0; i < model->GetMesh((0))->data[0].vertices.rows(); i++)
    {
        auto v = model->GetMesh((0))->data[0].vertices.row(i);
        projectedVertices.push_back((Eigen::Vector2d(v.x(), v.z())));
    }
    double xmin = std::numeric_limits<double>::max();
    double xmax = -std::numeric_limits<double>::max();
    double zmin = std::numeric_limits<double>::max();
    double zmax = -std::numeric_limits<double>::max();

    for (const Eigen::Vector2d& v : projectedVertices) {
        xmin = std::min(xmin, v.x());
        xmax = std::max(xmax, v.x());
        zmin = std::min(zmin, v.y());
        zmax = std::max(zmax, v.y());
    }
    float xspan = xmax - xmin;
    float zspan = zmax - zmin;

    Eigen::MatrixXd ret(projectedVertices.size(), 2);
    for(int i = 0; i < projectedVertices.size(); i++)
    {
        Eigen::Vector2d uv((projectedVertices[i].x() - xmin) / xspan, (projectedVertices[i].y() -zmin) / zspan);
        ret.row(i) = uv.transpose();
    }
    return ret;


/*double xmin = std::numeric_limits<double>::max();
double xmax = -std::numeric_limits<double>::max();
double ymin = std::numeric_limits<double>::max();
double ymax = -std::numeric_limits<double>::max();
double zmin = std::numeric_limits<double>::max();
double zmax = -std::numeric_limits<double>::max();

for (int i = 0; i < model->GetMeshList().size(); i++) {
    auto mesh = model->GetMesh(i);
    for (size_t j = 3500; j < mesh->data[0].vertices.rows(); j++) {
        auto v = mesh->data[0].vertices.row(j);
        xmin = std::min(xmin, v.x());
        xmax = std::max(xmax, v.x());
        ymin = std::min(ymin, v.y());
        ymax = std::max(ymax, v.y());
        zmin = std::min(zmin, v.z());
        zmax = std::max(zmax, v.z());
    }
}

std::vector<Eigen::Vector2d> texCoords;
for (int i = 0; i < model->GetMeshList().size(); i++) {
    auto mesh = model->GetMesh(i);
    for (size_t j = 0; j < mesh->data[0].faces.rows(); j++) {
        auto face = mesh->data[0].faces.row(j);
        Eigen::Vector3d centroid(0.0f, 0.0f, 0.0f);
        for (int k = 0; k < 3; k++)
        {
            Eigen::Vector3d v = mesh->data[0].vertices.row(face(0, k));
            centroid += v;
        }
        centroid /= 3.0;
        for (int k = 0; k < 3; k++)
        {
            Eigen::Vector3d v = mesh->data[0].vertices.row(face(0, k));
            double dist = (v - centroid).norm();
            Eigen::Vector2d uv(dist, 0.0f);
            texCoords.push_back(uv);
        }
    }
}
double xspan = xmax - xmin;
double yspan = ymax - ymin;
double zspan = zmax - zmin;
Eigen::MatrixXd ret(texCoords.size(), 2);
for (size_t i = 0; i < texCoords.size(); i++) {
    Eigen::Vector2d uv(texCoords[i].x() / xspan, texCoords[i].z() / zspan);
    ret.row(i) = uv.transpose();
}
return ret;*/
}