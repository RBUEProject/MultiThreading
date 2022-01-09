// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MultithreadingCharacter.generated.h"

UCLASS(config=Game)
class AMultithreadingCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AMultithreadingCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	/////////////////////////////////////////////////////////////////////////////////
	//游戏线程执行
	UFUNCTION(BlueprintCallable,Category = MultiThreading)
	void CaculatePrimeNumbers();
	//其他线程执行
	UFUNCTION(BlueprintCallable,Category = MultiThreading)
	void CaculatePrimeNumberAsync();

	UPROPERTY(EditAnywhere,Category = MultiThreading)
	int32 MaxPrime;
	//////////////////////////////////////////////////////////////////////////////////
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

namespace ThreadingTest
{
	static void CaculatePrimeNumbers(int32 UpperLimit)
	{
		for (int32 i = 1; i <= UpperLimit; ++i)
		{
			bool isPrime = true;
			for (int32 j = 2; j <= i/2; ++j)
			{
				if (FMath::Fmod(i, j) == 0)//对浮点数进行取模（求余）
				{
					isPrime = false;
					break;
				}
			}
			if(isPrime)GLog->Log("Prime number #"+FString::FromInt(i)+":"+FString::FromInt(i));
		}
	}
}

class PrimeCaculationAsyncTask : public FNonAbandonableTask
{
	int32 m_MaxPrime;

public:
	PrimeCaculationAsyncTask(int32 MaxPrime)
	{
		m_MaxPrime = MaxPrime;
	}

	FORCEINLINE TStatId GetStatId()const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(PrimeCaculationAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	void DoWork()
	{
		ThreadingTest::CaculatePrimeNumbers(m_MaxPrime);
		GLog->Log("--------------------------------------------------------------------");
		GLog->Log("End of prime numbers calculation on background thread");
		GLog->Log("--------------------------------------------------------------------");
	}
};