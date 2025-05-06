#include "CustomFighter_lobby.h"

#include "CustomFighter_save.h"
#include "CustomFighter_video.h"
#include "CustomFighter_input_interpreter.h"
#include "CustomFighter_net_tools.h"

using std::vector, std::string;

bool Lobby::Room::good() {
    return player_count == player_max;
}

Lobby::Room getRoomFromJSON(nlohmann::json json) {
    Lobby::Room room;

    if(json["code"].is_number_integer())
        room.code = json["code"];   

    if(json["player_max"].is_number_integer()) 
        room.player_max = json["player_max"];

    if(json["round_max"].is_number_integer()) 
        room.round_max = json["round_max"];

    if(json["timer_max"].is_number_integer()) 
        room.timer_max = json["timer_max"];

    if(json["game_mode"].is_number_integer()) 
        room.game_mode = json["game_mode"];

    if(json["player_count"].is_number_integer())
        room.player_count = json["player_count"];

    for(int i = 0; i < json["player_data"].size(); i ++) {
        auto& player_data = json["player_data"][i];

        if(player_data["remote"].is_string())  
            room.player_data[i].remote = player_data["remote"];

        if(player_data["config"].is_string())  
            room.player_data[i].config.loadFromText(player_data["config"]);
    }

    return room;
}

vector<Lobby::Room> fetchRooms(){
    vector<Lobby::Room> rooms;

    nlohmann::json postFields;
    postFields["reason"] = "list-room";
    postFields["remote"] = g::save.getNetworkAddress();

    nlohmann::json msg = runQuery(g::save.getServer(0), postFields);

    if(msg["response"].is_string() && msg["response"] == "OK") {

        for(auto& room : msg["rooms"]) 
            rooms.push_back(getRoomFromJSON(room));
    }
    return rooms;
}

int createRoom(int player_max, int round_max, int timer_max, int game_mode, string password=""){
    nlohmann::json postFields;
    postFields["reason"] = "create-room";
    postFields["password"] = password;

    postFields["player_max"] = player_max;
    postFields["round_max"] = round_max;
    postFields["timer_max"] = timer_max;
    postFields["game_mode"] = game_mode;
    
    nlohmann::json msg = runQuery(g::save.getServer(0), postFields);

    if(msg["response"].is_string() && msg["response"] == "OK") {
    
        if(msg["code"].is_number_integer())
            return msg["code"];
    }
    return -1;
}

bool joinRoom(int code, string password, Player::Config config, int slot = -1){
    nlohmann::json postFields;
    postFields["reason"] = "join-room";
    postFields["code"] = code;
    postFields["password"] = password;
    postFields["slot"] = slot;
    postFields["remote"] = g::save.getNetworkAddress();
    postFields["config"] = config.saveToText();

    nlohmann::json msg = runQuery(g::save.getServer(0), postFields);

    if(msg["response"].is_string() && msg["response"] == "OK") {
        return true;
    }
    return false;
}

bool leaveRoom(int code) {
    nlohmann::json postFields;
    postFields["reason"] = "leave-room";
    postFields["code"] = code;
    postFields["remote"] = g::save.getNetworkAddress();

    nlohmann::json msg = runQuery(g::save.getServer(0), postFields);

    if(msg["response"].is_string() && msg["response"] == "OK") {
        return true;
    }
    return false;   
}

bool statusRoom(Lobby::Room& room){
    nlohmann::json postFields;
    postFields["reason"] = "status-room";
    postFields["code"] = room.code;
    postFields["remote"] = g::save.getNetworkAddress();

    nlohmann::json msg = runQuery(g::save.getServer(0), postFields);

    if(msg["response"].is_string() && msg["response"] == "OK") {
        room = getRoomFromJSON(msg);
        return true;
    }
    return false;
}

Lobby::Room Lobby::run(Player::Config config) {

    // Room listings
    vector<Lobby::Room> roomList;
    int refreshTimer = 0;

    // Room joining / creation options
    Room join;
    int formatOption = 0;

    bool stayOpen = true; 

    while (g::video.isOpen() && stayOpen) {
        g::input.pollEvents();

        g::video.clear();

        ImGui::SetNextWindowPos({64, 64});
        ImGui::SetNextWindowSize({(float)g::video.getSize().x - 128, (float)g::video.getSize().y - 128});
        ImGui::Begin("Browser", &stayOpen);

        // Room Browser
        if(join.code == -1) {

            // List all rooms
            if(ImGui::BeginTable("Lobbies", 5)) {

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TableHeader("Code");
                ImGui::TableSetColumnIndex(1);
                ImGui::TableHeader("GameMode");
                ImGui::TableSetColumnIndex(2);
                ImGui::TableHeader("Players");
                ImGui::TableSetColumnIndex(3);
                ImGui::TableHeader("Password");

                for(auto& room : roomList) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%d", room.code);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", GameMode::String[room.game_mode].c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d/%d", room.player_count, room.player_max);
                    
                    // Password
                    ImGui::TableSetColumnIndex(3);
                    ImGui::InputText(("##Password" + std::to_string(room.code)).c_str(), &room.password, ImGuiInputTextFlags_Password);

                    // Attempt room
                    ImGui::TableSetColumnIndex(4);
                    if(ImGui::Button(("Join##" + std::to_string(room.code)).c_str()) ) {

                        if(joinRoom(room.code, room.password, config)) {
                            join.code = room.code;

                            if(!statusRoom(join))
                                join.code = -1;
                        }
                    }
                }
                ImGui::EndTable();
            }

            // Refresh all rooms
            if(ImGui::Button("Refresh")) {
                roomList = fetchRooms();
            }

            // Create room
            if(ImGui::Button("Create")) {
                int code = createRoom(join.player_max, join.round_max, join.timer_max, join.game_mode, join.password);

                if(joinRoom(code, join.password, config)) {
                    join.code = code;
                    
                    if(!statusRoom(join))
                        join.code = -1;
                }
            }

            // Drop down player_max
            const char* formats[] = {"1v1", "2v2"};
            if(ImGui::Combo("Format", &formatOption, formats, 2))
                join.player_max = (formatOption + 1) * 2;

            ImGui::InputText("Password", &join.password, ImGuiInputTextFlags_Password);

            // Round Max
            ImGui::InputInt("Rounds", &join.round_max);
            join.round_max = std::clamp(join.round_max, 1, 30);

            // Timer Max
            ImGui::InputInt("Round Time", &join.timer_max);
            join.timer_max = std::clamp(join.timer_max, 30, 99);

            // Drop down game mode
            if(ImGui::BeginCombo("GameMode", GameMode::String[join.game_mode].c_str())) {

                for(int i = 0; i < GameMode::Total; i ++) {

                    if(ImGui::Selectable(GameMode::String[i].c_str(), i == join.game_mode))
                        join.game_mode = i;
                }
                ImGui::EndCombo();
            }

        // Room viewer
        }else {
            ImGui::Text("Room Code: %d", join.code);
            ImGui::Text("GameMode: %s", GameMode::String[join.game_mode].c_str());
            ImGui::Text("Max Players: %d", join.player_max);
            ImGui::Text("Rounds: %d", join.round_max);
            ImGui::Text("Round Time: %d", join.timer_max);

            if(ImGui::Button("Leave")) {

                if(leaveRoom(join.code)) 
                    join.code = -1;
            }

            // Refresh all players within the room
            refreshTimer --;

            if(refreshTimer < 0) {
                refreshTimer = 150;

                if(!statusRoom(join))
                    join.code = -1;
            }

            // Show all players in the room
            if(ImGui::BeginTable("Players", 2)) {

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TableHeader("Remote");
                ImGui::TableSetColumnIndex(1);
                ImGui::TableHeader("Config");

                for(auto& player : join.player_data) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", player.remote.c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", player.config.saveToText().c_str());                    
                }
                ImGui::EndTable();
            }
        }

        ImGui::End();
        g::video.display();

        // Found the wanted players
        if(join.code != -1 && join.player_count == join.player_max) 
            return join;
    }
	return join;
}