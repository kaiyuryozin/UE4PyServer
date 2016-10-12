
#include "PyServerPrivatePCH.h"
#include "Engine.h"

extern "C"{
void* StrToPtr(const char* str)
{
	void* ptr;
	sscanf(str,"%p",&ptr);
	return ptr;
}

ULevel* GetCurrentLevel(UWorld* uworld)
{
	return uworld->GetCurrentLevel();		
}

int GetNumberOfLevelBluePrints(ULevel* level)
{
	TArray < class UBlueprint * > tarray= level->GetLevelBlueprints();
	return tarray.Num();
}

UBlueprint * GetLevelIthBluePrint(ULevel* level,int index)
{
	TArray < class UBlueprint * > tarray= level->GetLevelBlueprints();
	return tarray[index];	
}

int BlueprintGetFriendlyName(UBlueprint *bp,uint8* dstname,int dstsize)
{
	FString ret=bp->GetFriendlyName();
	return ret.ToBlob(ret,dstname,dstsize);
}

int GetActorCount(UWorld* uworld)
{
	return uworld->GetActorCount();
}

AActor* FindActorByName(UWorld* uworld,char* name,int verbose)
{
	FString fname(name);
	int i=0;
	for (TActorIterator<AActor> ActorItr(uworld); ActorItr;++ActorItr)
	{
		AActor *actor = *ActorItr;
		if(fname==ActorItr->GetName()) return actor;
		//* msg=ActorItr->GetName().GetCharArray();
		if(verbose) UE_LOG(LogTemp, Warning, TEXT("Actor(%d): %s"),i,*ActorItr->GetName());
		//ClientMessage(ActorItr->GetActorLocation().ToString());
		i++;
	}	
	return NULL;
}

void GetActorLocation(AActor* actor,float* outvec)
{
	FVector loc=actor->GetActorLocation();
	for(int i=0;i<3;i++) outvec[i]=loc[i];
}

void SetActorLocation(AActor* actor,float* invec)
{
	actor->SetActorLocation(FVector(invec[0],invec[1],invec[2]));
}

void GetActorRotation(AActor* actor,float* outvec)
{
	FRotator rot=actor->GetActorRotation();
	outvec[0]=rot.Pitch;outvec[1]=rot.Yaw;outvec[2]=rot.Roll;
}

void SetActorRotation(AActor* actor,float* invec)
{
	actor->SetActorRotation(FRotator(invec[0],invec[1],invec[2]));
}

void MoveToCameraActor(AActor* actor,ACameraActor* camera)
{
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(actor,0);
	OurPlayerController->SetViewTarget(camera);
}

void RequestScreenshot(const char* fname,bool bInShowUI,bool bAddFilenameSuffix)
{
	FString filename(fname);
	FScreenshotRequest::RequestScreenshot(filename, bInShowUI, bAddFilenameSuffix);

}

void RequestScreenshot2(UWorld* uworld,const char* fname)
{
	UGameViewportClient* gameViewport = uworld->GetGameViewport();
	//UGameViewportClient* gameViewport = GEngine->GameViewport;
	FViewport* InViewport = gameViewport->Viewport;
	TArray<FColor> Bitmap;
	FIntRect Rect(0, 0, InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y);
	bool bScreenshotSuccessful = GetViewportScreenShot(InViewport, Bitmap, Rect);
	//bool bScreenshotSuccessful = (InViewport, Bitmap, Rect);
	if (bScreenshotSuccessful){
		FIntVector Size(InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y, 0);
		TArray<uint8> CompressedBitmap;
		FString ScreenShotName(fname);
		FImageUtils::CompressImageArray(Size.X, Size.Y, Bitmap, CompressedBitmap);
		FFileHelper::SaveArrayToFile(CompressedBitmap, *ScreenShotName);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("bScreenshotSuccessful=False InViewport=%p"),reinterpret_cast<void*>(InViewport));
	}

}

void GetViewPortSize(int out_sz[2])
{
        UGameViewportClient* gameViewport = GEngine->GameViewport;
        FViewport* InViewport = gameViewport->Viewport;
	out_sz[0]=InViewport->GetSizeXY().X;
	out_sz[1]=InViewport->GetSizeXY().Y;
}

int TakeScreenshot(void* out_ptr,int length)
{
	UGameViewportClient* gameViewport = GEngine->GameViewport;
	FViewport* InViewport = gameViewport->Viewport;
	TArray<FColor> Bitmap;
	FIntRect Rect(0, 0, InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y);
	bool bScreenshotSuccessful = GetViewportScreenShot(InViewport, Bitmap, Rect);
	if (bScreenshotSuccessful){
		int sx=InViewport->GetSizeXY().X;
		int sy=InViewport->GetSizeXY().Y;
		TArray<uint8> CompressedBitmap;
		FImageUtils::CompressImageArray(sx, sy, Bitmap, CompressedBitmap);
		int cnt_non_zero=0;
		check(CompressedBitmap.Num()<=length)
	//	for(int i=0;i<CompressedBitmap.Num();i++) {
	//		out_ptr[i]=CompressedBitmap[i];
	//	}
		memcpy(out_ptr,reinterpret_cast<void*>(CompressedBitmap.GetData()),CompressedBitmap.Num());
		return CompressedBitmap.Num();

	} else {
		UE_LOG(LogTemp, Warning, TEXT("bScreenshotSuccessful=False InViewport=%p"),reinterpret_cast<void*>(InViewport));
	}
	return 0;
}

int TakeScreenshot2(void* out_ptr,int length)
{
	UGameViewportClient* gameViewport = GEngine->GameViewport;
	FViewport* InViewport = gameViewport->Viewport;
	TArray<FColor> Bitmap;
	FIntRect Rect(0, 0, InViewport->GetSizeXY().X, InViewport->GetSizeXY().Y);
	bool bScreenshotSuccessful = GetViewportScreenShot(InViewport, Bitmap, Rect);
	if (bScreenshotSuccessful){
		int sx=InViewport->GetSizeXY().X;
		int sy=InViewport->GetSizeXY().Y;
		check((Bitmap.Num()*4)<=length)
	//	for(int i=0;i<CompressedBitmap.Num();i++) {
	//		out_ptr[i]=CompressedBitmap[i];
	//	}
		memcpy(out_ptr,reinterpret_cast<void*>(Bitmap.GetData()),Bitmap.Num()*4);
		return Bitmap.Num();

	} else {
		UE_LOG(LogTemp, Warning, TEXT("bScreenshotSuccessful=False InViewport=%p"),reinterpret_cast<void*>(InViewport));
	}
	return 0;
}




void SetScreenResolution(int x,int y)
{
	//UGameUserSettings* UserSettingsObj = GEngine->GetGameUserSettings();
	//UserSettingsObj->SetScreenResolution(FIntPoint(x,y));
	UGameViewportClient* gameViewport = GEngine->GameViewport;
	//UGameViewportClient* gameViewport = GEngine->GameViewport;
	FViewport* InViewport = gameViewport->Viewport;
	FViewportFrame* ViewportFrame=InViewport->GetViewportFrame();
	ViewportFrame->ResizeFrame(640,480,EWindowMode::Windowed);

}


} //extern "C"
