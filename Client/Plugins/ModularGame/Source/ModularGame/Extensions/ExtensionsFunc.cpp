// Fill out your copyright notice in the Description page of Project Settings.


#include "ExtensionsFunc.h"

FDateTime ExtensionsFunc::ConvertUTCToLocalTime(FDateTime UTCDateTime)
{
	//Ŭ���̾�Ʈ�� ���� ���� �ð��� UTC �ð��� ���Ѵ�.
	FDateTime LocalNow = FDateTime::Now();
	FDateTime UtcNow = FDateTime::UtcNow();

	//�� �ð��� ���̸� ���ϸ�, ���� �ý����� �ð� �������� ��
	FTimespan LocalOffset = LocalNow - UtcNow;

	//���� UTC �ð��� ���� �������� ���ϸ� Ŭ���̾�Ʈ�� ���� �ð��� ��
	FDateTime ClientLocalTime = UTCDateTime + LocalOffset;

	return ClientLocalTime;
}
