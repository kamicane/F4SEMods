package
{
	import Shared.AS3.BSUIComponent;

	public class Background extends BSUIComponent
	{

		public function Background()
		{
			super();
			this.bShowBrackets = true;
			ShadedBackgroundMethod = "Shader";
			ShadedBackgroundType = "normal";
			this.bUseShadedBackground = true;
			this.BracketStyle = "vertical";
		}
	}
}
