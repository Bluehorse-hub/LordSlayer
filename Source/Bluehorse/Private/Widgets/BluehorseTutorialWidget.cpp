// Shungen All Rights Reserved.


#include "Widgets/BluehorseTutorialWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "BluehorseFunctionLibrary.h"
#include "Widgets/BluehorseWidgetBase.h"
#include "Kismet/GameplayStatics.h"

void UBluehorseTutorialWidget::NativeConstruct()
{
    Super::NativeConstruct();

    UE_LOG(LogTemp, Warning, TEXT("[TutorialWidget] NativeConstruct START"));

    // チュートリアル中はゲームをポーズ
    if (UWorld* World = GetWorld())
    {
        UGameplayStatics::SetGamePaused(World, true);
    }

    UE_LOG(LogTemp, Warning, TEXT("[TutorialWidget] Game Paused on Construct"));

    // チュートリアル以外のUIは非表示にする
    HideOtherWidgets();

    UE_LOG(LogTemp, Warning, TEXT("[TutorialWidget] NativeConstruct END"));
}

FReply UBluehorseTutorialWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    if (InKeyEvent.GetKey() == EKeys::Gamepad_FaceButton_Right)
    {
        PlayUISound(RemoveSound);

        // チュートリアルを読み終えたので破棄
        RemoveFromParent();

        // 非表示にしていたUIを表示する
        RestoreOtherWidgets();

        const bool bSucceeded = UGameplayStatics::SetGamePaused(GetWorld(), false);

        // 入力をここで消費し、他に伝播させない
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UBluehorseTutorialWidget::HideOtherWidgets()
{
    // --- Start Log ---
    UE_LOG(LogTemp, Warning, TEXT("[TutorialWidget] HideOtherWidgets START"));

    // 取得したWidgetを格納する配列
    TArray<UUserWidget*> FoundWidgets;

    // Viewport上に存在するUBluehorseWidgetBase派生のWidgetをすべて取得
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
        GetWorld(),
        FoundWidgets,
        UBluehorseWidgetBase::StaticClass(),
        true
    );

    UE_LOG(LogTemp, Warning,
        TEXT("[TutorialWidget] FoundWidgets Num = %d"),
        FoundWidgets.Num()
    );

    // 取得したWidgetを1つずつ処理
    for (UUserWidget* Widget : FoundWidgets)
    {
        // NULLチェック（安全対策）
        if (!Widget)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[TutorialWidget] Widget is NULL"));
            continue;
        }

        // BluehorseWidgetBaseにキャスト
        if (UBluehorseWidgetBase* BluehorseWidget = Cast<UBluehorseWidgetBase>(Widget))
        {
            UE_LOG(LogTemp, Verbose,
                TEXT("[TutorialWidget] Found BluehorseWidget : %s"),
                *BluehorseWidget->GetName()
            );

            // 自分自身（TutorialWidget）以外を非表示にする
            if (BluehorseWidget != this)
            {
                UE_LOG(LogTemp, Warning,
                    TEXT("[TutorialWidget] HIDE OTHER : %s"),
                    *BluehorseWidget->GetName()
                );

                // 他のWidgetを一時的に非表示
                BluehorseWidget->SetVisibility(ESlateVisibility::Hidden);
            }
            else
            {
                // 自分自身は表示を維持
                UE_LOG(LogTemp, Verbose,
                    TEXT("[TutorialWidget] Skip (Self Widget) : %s"),
                    *BluehorseWidget->GetName()
                );
            }
        }
    }
    // --- End Log ---
    UE_LOG(LogTemp, Warning, TEXT("[TutorialWidget] HideOtherWidgets END"));
}

void UBluehorseTutorialWidget::RestoreOtherWidgets()
{
    // --- Start Log ---
    UE_LOG(LogTemp, Warning, TEXT("[TutorialWidget] RestoreOtherWidgets START"));

    // 取得したWidgetを格納する配列
    TArray<UUserWidget*> FoundWidgets;

    // Viewport上に存在するUBluehorseWidgetBase派生のWidgetをすべて取得
    UWidgetBlueprintLibrary::GetAllWidgetsOfClass(
        GetWorld(),
        FoundWidgets,
        UBluehorseWidgetBase::StaticClass(),
        true
    );

    // 取得できたWidget数をログ出力
    UE_LOG(LogTemp, Warning,
        TEXT("[TutorialWidget] FoundWidgets Num = %d"),
        FoundWidgets.Num()
    );

    // 取得したWidgetを1つずつ処理
    for (UUserWidget* Widget : FoundWidgets)
    {
        // NULLチェック（安全対策）
        if (!Widget)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[TutorialWidget] Widget is NULL"));
            continue;
        }

        // BluehorseWidgetBaseにキャスト
        if (UBluehorseWidgetBase* BluehorseWidget = Cast<UBluehorseWidgetBase>(Widget))
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[TutorialWidget] RESTORE Widget : %s"),
                *BluehorseWidget->GetName()
            );

            // 非表示にしていたWidgetを表示状態に戻す
            BluehorseWidget->SetVisibility(ESlateVisibility::Visible);
        }
    }
    // --- End Log ---
    UE_LOG(LogTemp, Warning, TEXT("[TutorialWidget] RestoreOtherWidgets END"));
}
