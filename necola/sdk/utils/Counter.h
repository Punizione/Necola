
#pragma once

class Necola_Counter
{
public:
    void resetAll();
    void resetFirst(int weaponId);
    void resetSecond(int weaponId);
    static int firstSpecial();
    static int secondSpecial();
    void count(int weapon, int infected, int headshot);

public:
    static int weapon_first;
    static int weapon_second;

    static int first;
    static int first_commons;
    static int first_commons_headshot;
    static int first_special_headshot;

    static int first_boomer;
    static int first_charger;
    static int first_hunter;
    static int first_jockey;
    static int first_smoker;
    static int first_spitter;
    static int first_tank;
    static int first_witch;

    static int second;
    static int second_commons;
    static int second_commons_headshot;
    static int second_special_headshot;

    static int second_boomer;
    static int second_charger;
    static int second_hunter;
    static int second_jockey;
    static int second_smoker;
    static int second_spitter;
    static int second_tank;
    static int second_witch;


};
class Necola_BackupCounter
{   
public:
    static int weapon_first;
    static int weapon_second;

    static int first;
    static int first_commons;
    static int first_commons_headshot;
    static int first_special_headshot;

    static int first_boomer;
    static int first_charger;
    static int first_hunter;
    static int first_jockey;
    static int first_smoker;
    static int first_spitter;
    static int first_tank;
    static int first_witch;

    static int second;
    static int second_commons;
    static int second_commons_headshot;
    static int second_special_headshot;

    static int second_boomer;
    static int second_charger;
    static int second_hunter;
    static int second_jockey;
    static int second_smoker;
    static int second_spitter;
    static int second_tank;
    static int second_witch;

};


namespace G { inline Necola_Counter NecolaCounter; }
namespace G { inline Necola_BackupCounter NecolaBackupCounter; }