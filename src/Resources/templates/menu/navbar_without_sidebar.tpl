<div class="main-header navbar navbar-expand-md {$main_header_classes}">

    <div class="d-flex w-100 justify-content-between flex-wrap flex-md-nowrap">
        <button style="max-height: 40px;" class="navbar-toggler order-1" type="button" data-toggle="collapse" data-target="#navbarCollapse"
                aria-controls="navbarCollapse" aria-expanded="false" aria-label="Toggle navigation">
            <span class="navbar-toggler-icon"></span>
        </button>

        <div class="collapse navbar-collapse order-3 order-md-2" id="navbarCollapse">

            <!-- Left navbar links -->
            <ul class="navbar-nav navbar-left">

                <li class="nav-item nav-brand-logo mt-2 d-none d-md-block">
                    <a href="{$dispatcher}" class="mr-1">
                        <img src="{$brand_logo}" alt="ATK Logo" class="brand-image img-circle"
                             style="opacity: .8; max-width: 35px;">
                    </a>
                </li>

                {$menu['left']}
            </ul>

        </div>

        <!-- Right navbar links -->
        <ul class="navbar-nav navbar-no-expand ml-auto navbar-right order-2 order-md-3">
            {$menu['right']}

            <li class="nav-item d-none">
                <a class="nav-link" data-widget="fullscreen" href="#" role="button">
                    <i class="fas fa-expand-arrows-alt"></i>
                </a>
            </li>

        </ul>

    </div>
</div>


<script>

    //Check if element is hovered
    const isHover = e => e.parentElement.querySelector(':hover') === e;
    let lastHoveredElement = null;
    document.addEventListener('mousemove', function checkHover(evt) {
        const menuDropdownItem = evt.target.closest(".dropdown-item");
        if (menuDropdownItem != lastHoveredElement && menuDropdownItem != null) {
            parentItem = menuDropdownItem.parentElement;
            siblingMenu = menuDropdownItem.parentElement.querySelector(".dropdown-menu");
            if (siblingMenu != null && parentItem != null) {
                lastHoveredElement = menuDropdownItem;

                //Get parent distance from the right edge of the screen
                const parentDistanceFromRight = window.innerWidth - parentItem.getBoundingClientRect().right;

                //Get siblingMenu width
                const siblingMenuWidth = siblingMenu.getBoundingClientRect().width;

                //If the parent is too close to the right edge of the screen
                const childHasNoSpaceOnRight = parentDistanceFromRight < siblingMenuWidth;
                if (childHasNoSpaceOnRight) {
                    //Move the menu to the left
                    siblingMenu.style.left = -siblingMenuWidth + "px";
                    siblingMenu.style.top = "0";
                }

                //If the parent is too close to the left edge of the screen and there is no space on the right, show it in the bottom
                const childHasNoSpaceOnTheLeft = parentItem.getBoundingClientRect().left < siblingMenuWidth;
                if (childHasNoSpaceOnTheLeft && childHasNoSpaceOnRight) {
                    siblingMenu.style.top = "100%";
                    siblingMenu.style.left = "30px";
                }

                //If the parent has space on the right, show the menu on the right
                if (!childHasNoSpaceOnRight && !childHasNoSpaceOnTheLeft) {
                    siblingMenu.style.left = "100%";
                    siblingMenu.style.top = "0";
                }
            }
        }
    });
</script>