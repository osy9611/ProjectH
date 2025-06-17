// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtensionsFunc.h"

FDateTime ExtensionsFunc::ConvertUTCToLocalTime(FDateTime UTCDateTime)
{
	//클라이언트의 현재 로컬 시간과 UTC 시간을 구한다.
	FDateTime LocalNow = FDateTime::Now();
	FDateTime UtcNow = FDateTime::UtcNow();

	//두 시간의 차이를 구하면, 현재 시스템의 시간 오프셋이 됨
	FTimespan LocalOffset = LocalNow - UtcNow;

	//서버 UTC 시간에 로컬 오프셋을 더하면 클라이언트의 로컬 시간이 됨
	FDateTime ClientLocalTime = UTCDateTime + LocalOffset;

	return ClientLocalTime;
}
