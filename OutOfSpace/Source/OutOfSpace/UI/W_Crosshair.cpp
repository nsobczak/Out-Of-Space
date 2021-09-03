#include "OutOfSpace/UI/W_Crosshair.h"
#include "Animation/WidgetAnimation.h"

void UW_Crosshair::PlayLockAnimation(bool const bReverse)
{
	UUserWidget::PlayAnimation(LockAnimation, 0, 1,
	                           (bReverse ? EUMGSequencePlayMode::Reverse : EUMGSequencePlayMode::Reverse));
}
