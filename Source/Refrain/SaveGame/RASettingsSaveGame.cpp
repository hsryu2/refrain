// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveGame/RASettingsSaveGame.h"

URASettingsSaveGame::URASettingsSaveGame()
{
	// 기본 볼륨은 1.0 (100%)로 설정
	MasterVolume = 1.0f;
	BGMVolume = 1.0f;
	SFXVolume = 1.0f;
}
