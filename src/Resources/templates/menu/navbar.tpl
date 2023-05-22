<div class="main-header navbar navbar-expand {$main_header_classes}">

    <!-- Left navbar links -->
    <ul class="navbar-nav navbar-left">

        {if !$hide_sidebar}
            <!-- Minimize Sidebar Button -->
            <li class="nav-item nav-minimize-sidebar">
                <a class="nav-link" data-widget="pushmenu" href="#" role="button"><i class="fas fa-bars"></i></a>
            </li>
        {else}

            <!-- Brand Logo -->
            <li class="nav-item nav-brand-logo">
                <a href="{$dispatcher}" class="mr-1">
                    <img src="{$brand_logo}" alt="ATK Logo" class="brand-image img-circle"
                         style="opacity: .8; max-width: 35px;">
                </a>
            </li>
        {/if}

        {$menu['left']}

    </ul>

    <!-- Right navbar links -->
    <ul class="navbar-nav navbar-right ml-auto">
        {$menu['right']}

        <li class="nav-item d-none">
            <a class="nav-link" data-widget="fullscreen" href="#" role="button">
                <i class="fas fa-expand-arrows-alt"></i>
            </a>
        </li>

    </ul>
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