#include "Player.h"


bool titleTypeIntersection(const PlayerBorders borders, const std::set<short> &title_types,
                           const std::shared_ptr<TitleMap> &room_background_map, PointT &intersection) {
    // checking only corners is not enough
    for (int x = borders.x_left; x <= borders.x_right; ++x) {
        for (int y = borders.y_low; y <= borders.y_heigh; ++y) {
            short map_element = (*room_background_map)[y][x];
            if (title_types.count(map_element) != 0) {
                intersection = PointT{x, y};
                return true;
            }
        }
    }
    return false;
};

bool isBeyondWindow(const PlayerBorders borders) {
    if ((borders.y_heigh >= h_WINDOW_T_HEIGHT - 1) ||
        (borders.x_right >= h_WINDOW_T_WIDTH - 1) ||
        (borders.y_low <= 0) ||
        (borders.x_left <= 0)) {
        return true;
    }
    return false;
}

Pixel *makeMirrorPixelData(Pixel *data, int weight, int height) {
    Pixel *new_data = new Pixel[weight * height]{};
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < weight; ++x) {
            new_data[y * weight + x] = data[y * weight + weight - x - 1];
        }
    }
    return new_data;
}

Player::Player(const std::string &skins_path) {
    std::string path;
    std::ifstream skin_file(skins_path);
    if (!skin_file.is_open()) {
        std::cerr << "Unable to open title's path file " << skins_path << "\n";
        exit(4);
    }
    if (std::getline(skin_file, path)) {
        this->static_skin = std::make_shared<Image>(path);
    } else {
        std::cerr << "Skins file is empty\n";
        exit(4);
    }
    int i_width, i_height;
    while (std::getline(skin_file, path)) {
        this->dynamic_skins_right.push_back(std::make_shared<Image>(path));

//        std::shared_ptr<Image> image = std::make_shared<Image>(path);
//        this->dynamic_skins_right.push_back(image);
//        i_width = image->Width(), i_height = image->Height();
//        this->dynamic_skins_left.push_back(std::make_shared<Image>
//                                                   (makeMirrorPixelData(image->Data(), i_width, i_height),
//                                                    i_width,
//                                                    i_height,
//                                                    image->Channels())
//        );
    }
    skin_file.close();
    assert(!dynamic_skins_right.empty());
    width = 64;
    height = 44; // todo
};

PlayerBorders Player::GetTitleBorders(Point coord, int x_add_space = 0, int y_add_space = 0) {
    return PlayerBorders{
            (coord.x - x_add_space) / h_TEXTURE_SIZE,
            (coord.x + width + x_add_space) / h_TEXTURE_SIZE,
            (coord.y - y_add_space) / h_TEXTURE_SIZE,
            (coord.y + height + y_add_space) / h_TEXTURE_SIZE,
    };
};

void Player::ProcessInput(MovementDir dir, GlobalState &global_state) {
    int move_dist = move_speed * 1;
    Point tmp_old_coords{this->old_coords};
    Point tmp_coords{this->coords};
    this->is_moved = true;
    switch (dir) {
        case MovementDir::UP:
            tmp_old_coords.y = coords.y;
            tmp_coords.y += move_dist;
            break;
        case MovementDir::DOWN:
            tmp_old_coords.y = coords.y;
            tmp_coords.y -= move_dist;
            break;
        case MovementDir::LEFT:
            tmp_old_coords.x = coords.x;
            tmp_coords.x -= move_dist;
            break;
        case MovementDir::RIGHT:
            tmp_old_coords.x = coords.x;
            tmp_coords.x += move_dist;
            break;
        default:
            break;
    }
    movement.x += tmp_old_coords.x - tmp_coords.x;
    movement.y += tmp_old_coords.y - tmp_coords.y;

    PlayerBorders tmp_borders = GetTitleBorders(
            tmp_coords, -h_PLAYER_PHIS_WIDTH_SHIFT, -h_PLAYER_PHIS_HEIGHT_SHIFT);
//    std::cout<<"x: "<<tmp_coords.x<<" res= "<<tmp_borders.x_left<<", "<<tmp_borders.x_right<<"\n";
//    std::cout<<"y: "<<tmp_coords.y<<" res= "<<tmp_borders.y_low<<", "<<tmp_borders.y_heigh<<"\n";
//    std::cout<<"\n";
    PointT intersection;
    if (titleTypeIntersection(tmp_borders, h_lava, global_state.room_background_map, intersection)) {
        global_state.PushStateRoom(coords);
        return;
    }
    if (!isBeyondWindow(tmp_borders) &&
        !titleTypeIntersection(tmp_borders, h_walls, global_state.room_background_map, intersection)) {
        // update coordinates only if player not in the wall
        this->old_coords = tmp_old_coords;
        this->coords = tmp_coords;
    }
}

void Player::ProcessBridge(GlobalState &global_state) {
    int nearest_transition;
    double distance = detNearestPointT(
            coords, global_state.room_transitions_points, nearest_transition);
    if (distance < h_BRIDGE_REQ_DISTANCE) {
        global_state.PushStateBridge(nearest_transition);
        std::clog << "put bridge\n";
    }
}

void Player::Draw(Image &screen, GlobalState &screen_state) {
    updateSkin();
    if (is_moved) {
        for (int y = old_coords.y; y < old_coords.y + height; ++y) {
            for (int x = old_coords.x; x < old_coords.x + width; ++x) {
                screen.PutPixel(x, y, screen.GetPixel(x, y));
            }
        }
        old_coords = coords;
    }
    drawTrAsset(screen, dynamic_skins_right[skin_inx], coords.x, coords.y);
    is_moved = false;
}

void Player::SetPosition(Point player_position) {
    coords.x = player_position.x - width / 2;
    coords.y = player_position.y - height / 2;
}

bool Player::updateSkin() {
    if (!is_moved) { return false; }
    if (movement.x >= 8) {
        skin_inx = (skin_inx + 1) % dynamic_skins_right.size();
        movement.x = 0;
        return true;
    }
    if (movement.x <= -8) {
        skin_inx = (skin_inx + 1) % dynamic_skins_right.size();
        movement.x = 0;
        return true;
    }
    if (movement.y >= 8) {
        skin_inx = (skin_inx + 1) % dynamic_skins_right.size();
        movement.y = 0;
        return true;
    }
    if (movement.y <= -8) {
        skin_inx = (skin_inx + 1) % dynamic_skins_right.size();
        movement.y = 0;
        return true;
    }
    return false;
}


