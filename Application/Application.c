#include "Application.h"


void Application_Init(TextPage_t **TextPage, OLED_t *OLED)
{
  {
    static TextPage_t HomePage =
        {
            .Title = "Home",
            .ShowCallback = TextPage_ShowCallback,
            .UpdateCallback = TextPage_UpdateCallback,
        };
    *TextPage = &HomePage;

    static TextPage_t PageBack = TextPage_Back("<");
    TextPage_AddLowerPage(&HomePage, &PageBack);

    static TextPage_t Page1 =
        {
            .Title = "Page 1",
            .ShowCallback = TextPage_ShowCallback,
            .UpdateCallback = TextPage_UpdateCallback,
            .ClickCallback = TextPage_EnterCallback,
            .RotationCallback = TextPage_CursorCallback,
        };
    TextPage_AddLowerPage(&HomePage, &Page1);

    {
      static TextPage_t PageBack = TextPage_Back("<");
      TextPage_AddLowerPage(&Page1, &PageBack);

      static TextPage_t Page1_1 =
          {
              .Title = "Page 1-1",
              .RotationCallback = TextPage_CursorCallback,
          };
      TextPage_AddLowerPage(&Page1, &Page1_1);

      static TextPage_t Page1_2 =
          {
              .Title = "Page 1-2",
              .RotationCallback = TextPage_CursorCallback,
          };
      TextPage_AddLowerPage(&Page1, &Page1_2);
    }

    static TextPage_t Page2 =
        {
            .Title = "Page 2",
            .RotationCallback = TextPage_CursorCallback,
        };
    TextPage_AddLowerPage(&HomePage, &Page2);

    static TextPage_t Page3 =
        {
            .Title = "Page 3",
            .RotationCallback = TextPage_CursorCallback,
        };
    TextPage_AddLowerPage(&HomePage, &Page3);

    static TextPage_t Page4 =
        {
            .Title = "Page 4",
            .RotationCallback = TextPage_CursorCallback,
        };
    TextPage_AddLowerPage(&HomePage, &Page4);

    static TextPage_t Page5 =
        {
            .Title = "Page 5",
            .RotationCallback = TextPage_CursorCallback,
        };
    TextPage_AddLowerPage(&HomePage, &Page5);

    static TextPage_t Page6 =
        {
            .Title = "Page 6",
            .RotationCallback = TextPage_CursorCallback,
        };
    TextPage_AddLowerPage(&HomePage, &Page6);
  }

  TextPage_Init(*TextPage, OLED);
}
