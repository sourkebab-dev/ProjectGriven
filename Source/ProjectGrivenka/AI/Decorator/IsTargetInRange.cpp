// Fill out your copyright notice in the Description page of Project Settings.


#include "IsTargetInRange.h"

UIsTargetInRange::UIsTargetInRange() : UBTDecorator_IsAtLocation() {
	this->bAllowAbortChildNodes = true;
	this->bAllowAbortLowerPri = true;
	this->bAllowAbortNone = true;
}