#pragma once
#include "./utils/Counter.h"
#include "./l4d2/includes/const.h"



void Necola_Counter::resetAll()
{
    resetFirst(0);
    resetSecond(0);
}

void Necola_Counter::resetFirst(int weaponId)
{
    first = first_commons = first_boomer = first_charger = first_hunter = first_jockey = first_smoker = first_spitter = first_tank = first_witch = 0; 
    first_commons_headshot = first_special_headshot = 0;
    weapon_first = weaponId;
}

void Necola_Counter::resetSecond(int weaponId)
{
    second = second_commons = second_boomer = second_charger = second_hunter = second_jockey = second_smoker = second_spitter = second_tank = first_witch = 0;
    second_commons_headshot = second_special_headshot = 0;
    weapon_second = weaponId;
}

int Necola_Counter::firstSpecial()
{
    int specials = first - first_commons;
    return  specials;
}

int Necola_Counter::secondSpecial()
{
    int specials = second - second_commons;
    return  specials;
}

void Necola_Counter::count(int weapon, int infected, int headshot)
{
    if(weapon == WEAPON_PISTOL || weapon == WEAPON_MELEE || weapon == WEAPON_CHAINSAW || weapon == WEAPON_DEAGLE) {
        if(weapon == weapon_second) {
            switch (infected)
            {
                case Boomer:
                    second_boomer ++ ;
                    break;
                case Charger:
                    second_charger ++ ;
                    break;
                case Hunter:
                    second_hunter ++ ;
                    break;
                case Jockey:
                    second_jockey ++ ;
                    break;
                case Smoker:
                    second_smoker ++ ;
                    break;
                case Spitter:
                    second_spitter ++ ;
                    break;
                case Tank:
                    second_tank ++ ;
                    break;
                case Witch:
                    second_witch ++ ;
                    second_commons --;
                    second --;
                    break;
                case Infected:
                    second_commons ++;
                    break;
            }
            if(headshot == 1) {
                switch (infected)
                {
                    case Boomer:
                    case Charger:  
                    case Hunter:   
                    case Jockey:   
                    case Smoker:   
                    case Spitter:  
                    case Tank:  
                        second_special_headshot ++;
                        break;
                    case Witch:
                        // will never be triggered because witch_killed event dont have headshot attribute
                        // so the "special_headshot" and the "commons_headshot" maybe inaccurate
                        // but total count of special_headshot + commons_headshot is accurate
                        second_special_headshot ++;
                        second_commons_headshot --;
                        break;
                    case Infected:
                        second_commons_headshot ++;
                        break;
                }
            }
            second ++;
        }
        
    } else {
        if(weapon == weapon_first) {
            switch (infected)
            {
                case Boomer:
                    first_boomer ++ ;
                    break;
                case Charger:
                    first_charger ++ ;
                    break;
                case Hunter:
                    first_hunter ++ ;
                    break;
                case Jockey:
                    first_jockey ++ ;
                    break;
                case Smoker:
                    first_smoker ++ ;
                    break;
                case Spitter:
                    first_spitter ++ ;
                    break;
                case Tank:
                    first_tank ++ ;
                    break;
                case Witch:
                    first_witch ++ ;
                    first_commons --;
                    first -- ;
                    break;
                case Infected:
                    first_commons ++;
                    break;
                    
            }
            if(headshot == 1) {
                switch (infected)
                {
                    case Boomer:
                    case Charger:  
                    case Hunter:   
                    case Jockey:   
                    case Smoker:   
                    case Spitter:  
                    case Tank:
                        first_special_headshot ++;
                        break;
                    case Witch:
                        // will never be triggered because witch_killed event dont have headshot attribute
                        // so the "special_headshot" and the "commons_headshot" maybe inaccurate
                        // but total count of special_headshot + commons_headshot is accurate
                        first_special_headshot ++;
                        first_commons_headshot --;
                        break;
                    case Infected:
                        first_commons_headshot ++;
                        break;
                }
            
            }
            first ++;
        } 
    } 

}


int Necola_Counter::weapon_first = 0;
int Necola_Counter::weapon_second = 0;

int Necola_Counter::first = 0;
int Necola_Counter::first_commons = 0;
int Necola_Counter::first_commons_headshot = 0;
int Necola_Counter::first_special_headshot = 0;

int Necola_Counter::first_boomer = 0;
int Necola_Counter::first_charger = 0;
int Necola_Counter::first_hunter = 0;
int Necola_Counter::first_jockey = 0;
int Necola_Counter::first_smoker = 0;
int Necola_Counter::first_spitter = 0;
int Necola_Counter::first_tank = 0;
int Necola_Counter::first_witch = 0;

int Necola_Counter::second = 0;
int Necola_Counter::second_commons = 0;
int Necola_Counter::second_commons_headshot = 0;
int Necola_Counter::second_special_headshot = 0;

int Necola_Counter::second_boomer = 0;
int Necola_Counter::second_charger = 0;
int Necola_Counter::second_hunter = 0;
int Necola_Counter::second_jockey = 0;
int Necola_Counter::second_smoker = 0;
int Necola_Counter::second_spitter = 0;
int Necola_Counter::second_tank = 0;
int Necola_Counter::second_witch = 0;


int Necola_BackupCounter::weapon_first = 0;
int Necola_BackupCounter::weapon_second = 0;

int Necola_BackupCounter::first = 0;
int Necola_BackupCounter::first_commons = 0;
int Necola_BackupCounter::first_commons_headshot = 0;
int Necola_BackupCounter::first_special_headshot = 0;

int Necola_BackupCounter::first_boomer = 0;
int Necola_BackupCounter::first_charger = 0;
int Necola_BackupCounter::first_hunter = 0;
int Necola_BackupCounter::first_jockey = 0;
int Necola_BackupCounter::first_smoker = 0;
int Necola_BackupCounter::first_spitter = 0;
int Necola_BackupCounter::first_tank = 0;
int Necola_BackupCounter::first_witch = 0;

int Necola_BackupCounter::second = 0;
int Necola_BackupCounter::second_commons = 0;
int Necola_BackupCounter::second_commons_headshot = 0;
int Necola_BackupCounter::second_special_headshot = 0;

int Necola_BackupCounter::second_boomer = 0;
int Necola_BackupCounter::second_charger = 0;
int Necola_BackupCounter::second_hunter = 0;
int Necola_BackupCounter::second_jockey = 0;
int Necola_BackupCounter::second_smoker = 0;
int Necola_BackupCounter::second_spitter = 0;
int Necola_BackupCounter::second_tank = 0;
int Necola_BackupCounter::second_witch = 0;